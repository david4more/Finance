#include "Server.h"

#include <QtHttpServer/QtHttpServer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

QString Server::accountsTable = R"(
    CREATE TABLE accounts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL)
)";

QString Server::categoriesTable = R"(
    CREATE TABLE categories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    isExpense BOOLEAN NOT NULL,
    monthlyLimit INTEGER,
    color TEXT)
)";

QString Server::currenciesTable = R"(
    CREATE TABLE currencies (
    code TEXT PRIMARY KEY,
    rate REAL NOT NULL,
    symbol TEXT)
)";

QString Server::transactionsTable = R"(
    CREATE TABLE transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    amount REAL,
    dateTime TEXT,
    note TEXT,
    currency TEXT,
    category INTEGER,
    account INTEGER,
    FOREIGN KEY(currency) REFERENCES currencies(code),
    FOREIGN KEY(category) REFERENCES categories(id),
    FOREIGN KEY(account) REFERENCES accounts(id))
)";

Server::Server() : QObject(nullptr)
{
    const QString name = "main", path = "finance.db";

    if (QSqlDatabase::contains(name)) {
        qDebug() << "not first call";
        return;
    }

    bool isFirstLaunch = !QFile::exists(path);

    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(path);
    if (!db.open()) qDebug() << "Failed to open DB";

    if (isFirstLaunch) {
        createTables();
        setupDefaultCategories() && setupDefaultCurrencies() && setupDefaultAccounts();
    }

    baseCurrency = "EUR";
    apiKey = qgetenv("CURRENCY_API_KEY");

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    tcpServer->listen(QHostAddress::Any, 8080);

    qDebug() << "Listening...";
}

void Server::onNewConnection() {
    QTcpSocket* socket = tcpServer->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void Server::onReadyRead() {
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();
    QByteArray body = data.mid(data.indexOf("\r\n\r\n") + 4);
    QString sql = QJsonDocument::fromJson(body).object()["sql"].toString();

    QJsonObject result;

    if (sql == "PING") {                              // ← add here
        result[u"success"] = true;
    } else if (sql.startsWith("FETCH_RATES:")) {
        QStringList parts = sql.split(":");
        result = requestExchangeRates(parts[1], parts[2]);
    } else {
        result = executeQuery(sql);
    }

    QByteArray responseBody = QJsonDocument(result).toJson();
    socket->write(
        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
        + QByteArray::number(responseBody.size()) + "\r\n\r\n" + responseBody);
    socket->disconnectFromHost();
}
QJsonObject Server::executeQuery(QString query)
{
    QSqlQuery q(db);
    QJsonObject response;

    if (!q.exec(query)) {
        response[u"success"] = false;
        response[u"error"] = q.lastError().text();
        return response;
    }

    QJsonArray rows;
    while (q.next()) {
        QJsonObject row;
        for (int i = 0; i < q.record().count(); i++)
            row[q.record().fieldName(i)] = QJsonValue::fromVariant(q.value(i));
        rows.append(row);
    }

    response[u"success"] = true;
    response[u"rows"] = rows;
    response[u"affected"] = q.numRowsAffected();
    return response;
}

QJsonObject Server::requestExchangeRates(QString currencies, QString base)
{
    baseCurrency = base;
    QUrl url("https://api.currencyapi.com/v3/latest");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("currencies", currencies);
    urlQuery.addQueryItem("base_currency", baseCurrency);
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setRawHeader("apikey", apiKey.toUtf8());

    auto blockingGetRequest = [](const QNetworkRequest& request){
        int timeoutMs = 5000;

        auto mgr = new QNetworkAccessManager;

        QTimer timer;
        timer.setSingleShot(true);

        QEventLoop loop;
        QNetworkReply* reply = mgr->get(request);

        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        timer.start(timeoutMs);
        loop.exec();

        if (!timer.isActive()) { qDebug() << "Request took too long - " << timeoutMs << " ms.";
            reply->abort(); reply->deleteLater(); return QJsonObject{}; }
        timer.stop();

        if (reply->error() != QNetworkReply::NoError) { qDebug() << reply->errorString(); reply->deleteLater(); return QJsonObject{}; }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        mgr->deleteLater();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError) return QJsonObject{};
        return doc.object();
    };
    QJsonObject rootObj = blockingGetRequest(request); qDebug() << "CurrencyAPI was called";
    QJsonObject obj = rootObj["data"].toObject();

    QString lastUpdated = rootObj["meta"].toObject()["last_updated_at"].toString();
    lastUpdate = QDateTime::fromString(lastUpdated, Qt::ISODate);
    updated = true;

    QSqlQuery query(db);

    auto names = obj.keys();

    QJsonObject fail, ok;
    fail[u"success"] = false;
    ok[u"success"] = true;

    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return fail; }

    query.prepare("INSERT INTO currencies (code, rate, symbol) VALUES (:code, :rate, :symbol)");
    for (int i = 0; i < obj.size(); ++i) {
        QString code = names[i];
        double rate = obj[code].toObject()["value"].toDouble();
        QString symbol = code;

        query.bindValue(":code", code);
        query.bindValue(":rate", rate);
        query.bindValue(":symbol", QString(symbol));
        query.exec();
    }

    if (!db.commit()) return fail;
    return ok;
}

bool Server::createTables()
{
    QSqlQuery query(db);
    for (const auto& q : { categoriesTable, currenciesTable, accountsTable, transactionsTable })
        if (!query.exec(q)) { qDebug() << "Failed to create table"; return false; }

    return true;
}

bool Server::setupDefaultCurrencies()
{
    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    QSqlQuery query(db);
    query.prepare("INSERT INTO currencies (code, rate, symbol) VALUES (:code, :rate, :symbol)");

    query.bindValue(":code", "EUR");
    query.bindValue(":rate", 1.0000);
    query.bindValue(":symbol", QString(QChar(0x20AC)));
    query.exec();

    query.bindValue(":code", "GBP");
    query.bindValue(":rate", 0.8741);
    query.bindValue(":symbol", QString(QChar(0x00A3)));
    query.exec();

    query.bindValue(":code", "CHF");
    query.bindValue(":rate", 0.9356);
    query.bindValue(":symbol", QString(QChar(0x0043)));
    query.exec();

    query.bindValue(":code", "PLN");
    query.bindValue(":rate", 4.2248);
    query.bindValue(":symbol", QString(QChar(0x007A)));
    query.exec();

    query.bindValue(":code", "UAH");
    query.bindValue(":rate", 49.46);
    query.bindValue(":symbol", QString(QChar(0x20B4)));
    query.exec();

    query.bindValue(":code", "USD");
    query.bindValue(":rate", 1.1634);
    query.bindValue(":symbol", QString(QChar(0x0024)));
    query.exec();

    query.bindValue(":code", "CAD");
    query.bindValue(":rate", 1.6108);
    query.bindValue(":symbol", QString(QChar(0x0024)));
    query.exec();

    query.bindValue(":code", "JPY");
    query.bindValue(":rate", 182.32);
    query.bindValue(":symbol", QString(QChar(0x00A5)));
    query.exec();

    query.bindValue(":code", "CNY");
    query.bindValue(":rate", 8.2165);
    query.bindValue(":symbol", QString(QChar(0x00A5)));
    query.exec();

    if (!db.commit()) { qDebug() << "Failed to commit a transaction"; return false; }

    _baseCurrency = "EUR";

    return true;
}

bool Server::setupDefaultCategories()
{
    auto addCategory = [&](QString name, bool isExpense, int monthlyLimit = 1000, QString color = QString("#00ff00"))
    {
        QSqlQuery query(db);
        query.prepare("SELECT 1 FROM categories WHERE name = :name AND isExpense = :isExpense LIMIT 1");
        query.bindValue(":name", name);
        query.bindValue(":isExpense", isExpense? 1 : 0);

        if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::add query"; return false; }
        if (query.next()) { qDebug() << "Category already exists"; return false; }
        query.clear();

        query.prepare("INSERT INTO categories (name, isExpense, monthlyLimit, color) VALUES (:name, :isExpense, :monthlyLimit, :color)");

        query.bindValue(":name", name);
        query.bindValue(":isExpense", isExpense? 1 : 0);
        query.bindValue(":monthlyLimit", rand() % monthlyLimit);
        query.bindValue(":color", color);

        if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::add query"; return false; }

        return true;
    };

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM categories LIMIT 1");
    if (!query.exec()) { qDebug() << "Failed to execute query"; return false; }
    if (query.next()) { qDebug() << "Setup of non-empty table"; return false; }

    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    for (auto n : {"Food", "Entertainment", "Gifts", "Health", "Clothing", "Education", "Transport", "Household" })
        if (!addCategory(n, true)) { db.rollback(); return false; }

    for (auto n : { "Salary", "Help", "Bonuses", "Gifts" })
        if (!addCategory(n, false)) { db.rollback(); return false; }

    if (!db.commit()) { qDebug() << "Failed to commit transaction"; return false; }

    return true;
}

bool Server::setupDefaultAccounts()
{
    auto addAccount = [&](QString name){
        QSqlQuery query(db);
        query.prepare("INSERT INTO accounts (name) VALUES (:name)");
        query.bindValue(":name", name);

        if (!query.exec()) { qDebug() << "Failed to execute AccountsManager::add query"; return false; }

        return true;
    };

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM accounts LIMIT 1");
    if (!query.exec()) { qDebug() << "Failed to execute query"; return false; }
    if (query.next()) { qDebug() << "Setup of non-empty table"; return false; }

    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    for (auto n : { "Cash", "Card", "Crypto", "Family's savings", "Loan" })
        if (!addAccount(n)) { db.rollback(); return false; }

    if (!db.commit()) { qDebug() << "Failed to commit transaction"; return false; }

    return true;
}

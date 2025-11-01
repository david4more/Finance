#include "backend.h"

Backend::Backend(QObject* parent) : QObject(parent)
{
    ;
}

void Backend::init()
{
    if (db.isOpen()) {
        emit message("Initializing backend twice");
        return;
    }

    if (!QSqlDatabase::contains("main")) {
        db = QSqlDatabase::addDatabase("QSQLITE", "main");
        db.setDatabaseName(dbName);
    } else
        db = QSqlDatabase::database("main");

    if (!db.open()) {
        emit message("Failed to open DB: " + db.lastError().text());
        return;
    }

    auto execQueries = [&](const QStringList &queries) {
        QSqlQuery query(db);
        for (const auto& q : queries){
            if (!query.exec(q)){
                emit message("Failed to create table(s): " + query.lastError().text());
                return false;
            }
        }
        return true;
    };

    if (execQueries({Transaction::table(), Currencies::table(), Backend::accountsTable(), Backend::categoriesTable()}) && initLists())
        qDebug() << "DB ready to listen.";
    else
        emit message("Failed to setup database");
}

bool Backend::initLists()
{
    QStringList currencyCodes = {
        // Europe
        "EUR", // Euro
        "GBP", // British Pound
        "CHF", // Swiss Franc
        "PLN", // Polish Zloty
        "UAH", // Ukrainian Hryvnia
        // America
        "USD", // US Dollar
        "CAD", // Canadian Dollar
        // Asia
        "JPY", // Japanese Yen
        "CNY", // Chinese Yuan
    };
    for (auto cur : currencyCodes) {
        currencies.value(cur);
    }

    accounts = { "User1", "User2", "Muhehehehe" };
    categories = { "Food", "Entertainment", "Hehehoho", "Health", "Gifts" };
    return true;
}

void Backend::newTransaction(Transaction t)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO transactions (amount, currency, dateTime, category, account, note) "
        "VALUES (:amount, :currency, :dateTime, :category, :account, :note)"
        );

    query.bindValue(":amount", t.amount);
    query.bindValue(":currency", t.currency);
    query.bindValue(":dateTime", t.dateTime);
    query.bindValue(":category", t.category);
    query.bindValue(":account", t.account);
    query.bindValue(":note", t.note);

    if (!query.exec())
        throw std::runtime_error("Failed to execute query: " + query.lastError().text().toStdString());
    else
        qDebug() << "Query executed.";
}

QVector<Transaction> Backend::getTransactions(const QDate& from, const QDate& to)
{
    QVector<Transaction> transactions;

    QSqlQuery query(db);

    // SQLite date comparison works with 'YYYY-MM-DD' strings
    query.prepare("SELECT * "
                  "FROM transactions "
                  "WHERE date(dateTime) BETWEEN :from AND :to "
                  "ORDER BY date(dateTime) ASC");

    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));


    if (!query.exec()) {
        throw std::runtime_error("Failed to execute query: " + query.lastError().text().toStdString());
        return transactions;
    }


    while (query.next()) {
        Transaction t;
        t.amount    = query.value("amount").toDouble();
        t.currency  = query.value("currency").toString();
        t.dateTime  = QDateTime::fromString(query.value("dateTime").toString(), Qt::ISODate);
        t.category  = query.value("category").toString();
        t.account   = query.value("account").toString();
        t.note      = query.value("note").toString();
        t.id        = query.value("id").toInt();

        transactions.push_back(std::move(t));
    }

    return transactions;
}



























#include "CurrenciesManager.h"
#include "NetworkManager.h"
#include "../Modules/pch.h"
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonObject>
#include <QtWidgets/qmessagebox.h>
#include <QSqlError>

bool CurrenciesManager::init()
{
    _base = "EUR";
    QSqlQuery query(db);
    _apiKey = qgetenv("CURRENCY_API_KEY");
    // Q_ASSERT(!_apiKey.isEmpty());
    if (!query.exec("SELECT 1 FROM currencies LIMIT 1")) { qDebug() << query.lastError().text(); return false; }

    query.prepare("SELECT * FROM currencies");
    if (!query.exec()) { qDebug() << "Failed to execute CurrenciesManager::CurrenciesManager query"; return false; }

    while (query.next())
        _currencies.push_back({ query.value(0).toString(), query.value(1).toDouble(), query.value(2).toString() });

    return true;
}

bool CurrenciesManager::requestLatest(QString currencies, QString base)
{
    _base = base;
    QUrl url("https://api.currencyapi.com/v3/latest");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("currencies", currencies);
    urlQuery.addQueryItem("base_currency", _base);
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    request.setRawHeader("apikey", _apiKey.toUtf8());

    QJsonObject rootObj = NetworkManager::blockingNetworkRequest(request); qDebug() << "CurrencyAPI was called";
    QJsonObject obj = rootObj["data"].toObject();

    QString lastUpdated = rootObj["meta"].toObject()["last_updated_at"].toString();
    _lastUpdate = QDateTime::fromString(lastUpdated, Qt::ISODate);
    updated = true;

    QSqlQuery query(db);

    auto names = obj.keys();

    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    query.prepare("INSERT INTO currencies (code, rate, symbol) VALUES (:code, :rate, :symbol)");
    for (int i = 0; i < obj.size(); ++i) {
        QString code = names[i];
        double rate = obj[code].toObject()["value"].toDouble();
        QString symbol = code;
        _currencies.push_back({code, rate, symbol});

        query.bindValue(":code", code);
        query.bindValue(":rate", rate);
        query.bindValue(":symbol", QString(symbol));
        query.exec();
    }

    if (!db.commit()) { qDebug() << "Failed to commit a transaction"; return false; }

    return true;
}

bool CurrenciesManager::setupDefault()
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

    _base = "EUR";

    return true;
}

QMap<QString, double> CurrenciesManager::rates() const
{
    QMap<QString, double> ret;
    for (const auto& c : _currencies) ret.insert(c.code, c.rate);
    return ret;
}

QMap<QString, QChar> CurrenciesManager::symbols() const
{
    QMap<QString, QChar> ret;
    for (const auto& c : _currencies) ret.insert(c.code, c.symbol[0]);
    return ret;
}

QStringList CurrenciesManager::codes() const
{
    QStringList ret;
    for (const auto& c : _currencies) ret.push_back(c.code);
    return ret;
}

QString CurrenciesManager::base() const
{
    return _base;
}

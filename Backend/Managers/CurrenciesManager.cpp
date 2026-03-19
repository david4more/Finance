#include "CurrenciesManager.h"

#include <QJsonArray>

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

    auto res = NetworkManager::blockingSqlRequest("SELECT * FROM currencies");
    if (!res[u"success"].toBool()) { qDebug() << "Failed to execute CurrenciesManager::init query"; return false; }

    for (const auto& row : res[u"rows"].toArray())
        _currencies.push_back({ row[u"code"].toString(), row[u"rate"].toDouble(), row[u"symbol"].toString() });

    return true;
}

bool CurrenciesManager::requestLatest(QString currencies, QString base)
{
    _base = base;
    auto res = NetworkManager::blockingSqlRequest(
        QString("FETCH_RATES:%1:%2").arg(base, currencies));
    return res[u"success"].toBool();
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

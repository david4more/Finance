#include "AccountsManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include "NetworkManager.h"

bool AccountsManager::init()
{
    auto res = NetworkManager::blockingSqlRequest("SELECT name FROM accounts");
    if (!res[u"success"].toBool()) { qDebug() << "Failed to execute AccountsManager::init query"; return false; }

    for (const auto& row : res[u"rows"].toArray())
        names.push_back(row[u"name"].toString());

    return true;
}
int AccountsManager::findId(QString name) const
{
    auto res = NetworkManager::blockingSqlRequest(
        QString("SELECT id FROM accounts WHERE name = '%1'").arg(name));

    if (!res[u"success"].toBool()) return -1;

    auto rows = res[u"rows"].toArray();
    return rows.isEmpty() ? -1 : rows[0][u"id"].toInt();
}

bool AccountsManager::add(QString name)
{
    return NetworkManager::blockingSqlRequest(
        QString("INSERT INTO accounts (name) VALUES ('%1')").arg(name)
    )[u"success"].toBool();
}

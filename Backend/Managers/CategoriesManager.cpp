#include "CategoriesManager.h"

#include <QJsonArray>
#include <QJsonObject>

#include "../Modules/Category.h"
#include "../Managers/NetworkManager.h"

bool CategoriesManager::init()
{
    auto res = NetworkManager::blockingSqlRequest("SELECT id, name, isExpense, color FROM categories");
    if (!res[u"success"].toBool()) { qDebug() << "Failed to execute CategoriesManager::init query"; return false; }

    for (const auto& row : res[u"rows"].toArray())
        _categories.push_back(Category(
            row[u"id"].toInt(),
            row[u"name"].toString(),
            row[u"isExpense"].toBool(),
            row[u"color"].toString()));

    return true;
}
QMap<QString, double> CategoriesManager::getLimits() const
{
    auto res = NetworkManager::blockingSqlRequest("SELECT name, monthlyLimit FROM categories WHERE isExpense = 1");
    if (!res[u"success"].toBool()) return {};

    QMap<QString, double> ret;
    for (const auto& row : res[u"rows"].toArray())
        ret.insert(row[u"name"].toString(), row[u"monthlyLimit"].toDouble());
    return ret;
}

int CategoriesManager::findId(QString name, bool isExpense) const
{
    auto res = NetworkManager::blockingSqlRequest(
        QString("SELECT id FROM categories WHERE name = '%1' AND isExpense = %2").arg(name).arg(isExpense ? 1 : 0));

    if (!res[u"success"].toBool()) return -1;

    auto rows = res[u"rows"].toArray();
    return rows.isEmpty() ? -1 : rows[0][u"id"].toInt();
}

QStringList CategoriesManager::getNames(TransactionType type) const
{
    QString q = "SELECT name FROM categories";
    if (type == TransactionType::Expense) q += " WHERE isExpense = 1";
    else if (type == TransactionType::Income) q += " WHERE isExpense = 0";

    auto res = NetworkManager::blockingSqlRequest(q);
    if (!res[u"success"].toBool()) return {};

    QStringList ret;
    for (const auto& row : res[u"rows"].toArray())
        ret << row[u"name"].toString();
    return ret;
}

bool CategoriesManager::add(QString name, bool isExpense, int monthlyLimit, QString color)
{
    auto check = NetworkManager::blockingSqlRequest(
        QString("SELECT 1 FROM categories WHERE name = '%1' AND isExpense = %2 LIMIT 1").arg(name).arg(isExpense ? 1 : 0));

    if (!check[u"success"].toBool()) return false;
    if (!check[u"rows"].toArray().isEmpty()) { qDebug() << "Category already exists"; return false; }

    return NetworkManager::blockingSqlRequest(
        QString("INSERT INTO categories (name, isExpense, monthlyLimit, color) VALUES ('%1', %2, %3, '%4')")
            .arg(name).arg(isExpense ? 1 : 0).arg(rand() % monthlyLimit).arg(color)
    )[u"success"].toBool();
}

QVector<Category> CategoriesManager::get() const
{
    return _categories;
}
#include "CategoriesManager.h"
#include "../Modules/Category.h"
#include "../Modules/pch.h"

const QString CategoriesManager::defaultColor = "#00ff00";

QMap<QString, double> CategoriesManager::getLimits() const
{
    QSqlQuery query(db);

    if (!query.exec("SELECT name, monthlyLimit FROM categories WHERE isExpense = 1"))
        { qDebug() << "CategoriesManager::getLimits query failed"; return {}; }

    QMap<QString, double> ret;
    while (query.next()) ret.insert(query.value(0).toString(), query.value(1).toDouble());
    return ret;
}

int CategoriesManager::findId(QString name, bool isExpense) const
{
    QSqlQuery query(db);

    query.prepare("SELECT id FROM categories WHERE name = :name AND isExpense = :isExpense");
    query.bindValue(":name", name);
    query.bindValue(":isExpense", isExpense? 1 : 0);

    if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::lookUpId query"; return -1; }
    if (!query.next()) { qDebug() << "Category not found"; return -1; }

    return query.value(0).toInt();
}

bool CategoriesManager::init()
{
    QSqlQuery query(db);

    query.prepare("SELECT id, name, isExpense, color FROM categories");

    if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::init query"; return {}; }

    while (query.next())
        _categories.push_back(
            Category(query.value(0).toInt(), query.value(1).toString(), query.value(2).toBool(), query.value(3).toString()));

    return true;
}

QStringList CategoriesManager::getNames(TransactionType type) const
{
    QSqlQuery query(db);

    QString q = "SELECT name FROM categories";
    switch (type) {
    case TransactionType::Expense:
        q += " WHERE isExpense = 1"; break;
    case TransactionType::Income:
        q += " WHERE isExpense = 0"; break;
        default: break;
    }

    query.prepare(q);

    if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::getNames query"; return {}; }

    QStringList ret;
    while (query.next())
        ret << query.value(0).toString();

    return ret;
}

bool CategoriesManager::setupDefault()
{
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM categories LIMIT 1");
    if (!query.exec()) { qDebug() << "Failed to execute query"; return false; }
    if (query.next()) { qDebug() << "Setup of non-empty table"; return false; }

    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    for (auto n : {"Food", "Entertainment", "Gifts", "Health", "Clothing", "Education", "Transport", "Household" })
        if (!add(n, true)) { db.rollback(); return false; }

    for (auto n : { "Salary", "Help", "Bonuses", "Gifts" })
        if (!add(n, false)) { db.rollback(); return false; }

    if (!db.commit()) { qDebug() << "Failed to commit transaction"; return false; }

    return true;
}

bool CategoriesManager::add(QString name, bool isExpense, int monthlyLimit, QString color)
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
    query.bindValue(":monthlyLimit", monthlyLimit);
    query.bindValue(":color", color);

    if (!query.exec()) { qDebug() << "Failed to execute CategoriesManager::add query"; return false; }

    return true;
}

QVector<Category> CategoriesManager::get() const
{
    return _categories;
}

#pragma once

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QDateTime>
#include <QMap>
#include "transaction.h"

class Backend
{
public:
    Backend();
    void newTransaction(Transaction t);
    QVector<Transaction> getTransactions(const QDate& from, const QDate& to);
    const QStringList& getCurrencies() const { return currencies; }
    const QStringList& getCategories() const { return categories; }
    const QStringList& getAccounts() const { return accounts; }

private:
    bool initLists();

    QSqlDatabase db;
    const QString dbName = "finance.db";

    QStringList categories;
    QStringList currencies;
    QStringList accounts;

    static QString categoriesTable() {
        return "CREATE TABLE IF NOT EXISTS categories ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL)";
    }

    static QString currenciesTable() {
        return "CREATE TABLE IF NOT EXISTS currencies ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL,"
            "symbol TEXT UNIUE NOT NULL)";
    }

    static QString accountsTable() {
        return "CREATE TABLE IF NOT EXISTS accounts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL)";
    }
};

















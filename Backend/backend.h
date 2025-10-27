#pragma once

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QDateTime>
#include <QMap>

struct Transaction;
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

struct Transaction
{
    // recurrence
    float amount;
    QString currency;
    QDateTime dateTime;
    QString category;
    QString account;
    QString note;

    int id;
    static QString table() {
        return
            "CREATE TABLE IF NOT EXISTS transactions ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "amount REAL, "
            "currency TEXT, "
            "dateTime TEXT, "
            "category TEXT, "
            "account TEXT, "
            "note TEXT)";
    }

    explicit operator bool() const {
        return !(amount == 0.f || currency.isEmpty() || dateTime.isNull() || category.isEmpty() || account.isEmpty());
    }
};

















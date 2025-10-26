#pragma once

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QDateTime>

struct Transaction;
class Backend
{
public:
    Backend();
    void newTransaction(Transaction t);
    QVector<Transaction> getTransactions(const QDate& from, const QDate& to);
    QStringList getCurrencies() { return currencies; }
    QStringList getCategories() { return categories; }
    QStringList getAccounts() { return accounts; }

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
            "name TEXT UNIQUE NOT NULL)";
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
    QString name;
    float amount;
    QString currency;
    QDateTime dateTime;
    QString category;
    QString account;
    QString note;

    int id;
    static constexpr int fields = 7;
    static QString table() {
        return
            "CREATE TABLE IF NOT EXISTS transactions ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT, "
            "amount REAL, "
            "currency TEXT, "
            "dateTime TEXT, "
            "category TEXT, "
            "account TEXT, "
            "note TEXT)";
    }

    explicit operator bool() const {
        return !(name.isEmpty() || amount == 0.f || currency.isEmpty() || dateTime.isNull() || category.isEmpty() || account.isEmpty());
    }
};

















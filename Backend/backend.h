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

private:
    bool init();

    QSqlDatabase db;
    const QString dbName = "finance.db";
    QStringList categories;
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
};

struct Category
{
    int id;
    QString name;
    int limit;
};

















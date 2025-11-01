#pragma once

#include "transaction.h"
#include "currency.h"
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QDateTime>
#include <QMap>
#include <QNetworkAccessManager>

class Backend : public QObject
{
    Q_OBJECT
signals:
    void message(QString status);

public:
    Backend(QObject* parent = nullptr);
    void init();
    void newTransaction(Transaction t);
    QVector<Transaction> getTransactions(const QDate& from, const QDate& to);
    QStringList getCurrencies() const { return currencies.codes(); }
    const QStringList& getCategories() const { return categories; }
    const QStringList& getAccounts() const { return accounts; }
    QString defCurrency() const { return currencies.getDefaultCurrency(); }

private:
    bool initLists();
    void checkInit() { if (!initialized) emit message("Non-initialized backend usage"); };
    bool initialized = false;

    QSqlDatabase db;
    const QString dbName = "finance.db";

    QNetworkAccessManager network;

    Currencies currencies;
    QStringList categories;
    QStringList accounts;

    static QString categoriesTable() {
        return "CREATE TABLE IF NOT EXISTS categories ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL)";
    }

    static QString accountsTable() {
        return "CREATE TABLE IF NOT EXISTS accounts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL)";
    }
};

















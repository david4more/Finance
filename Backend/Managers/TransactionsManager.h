#pragma once

#include "../Modules/Utils.h"
class QSqlDatabase;
class Transaction;


class BACKEND_EXPORT TransactionsManager
{
    friend class Backend;

    QSqlDatabase& db;
    QString base = "EUR";

    [[nodiscard]] QVector<QPair<QString, double>> transactionsPerCategory(const QDate& from, const QDate& to, TransactionType type = TransactionType::All) const;
    [[nodiscard]] QVector<DailyTransactions> transactionsPerDay(const QDate& from, const QDate& to) const;
    [[nodiscard]] QVector<NamedTransactions> transactionsPerAccount(const QDate& from, const QDate& to) const;

    // convert into base currency

public:
    TransactionsManager(QSqlDatabase& db) : db(db) {}

    [[nodiscard]] QVector<Transaction> get(const QDate& from, const QDate& to, int limit = 0) const;
    bool add(const Transaction& t);
    bool clearTransactions();
};

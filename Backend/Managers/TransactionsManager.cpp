#include "TransactionsManager.h"
#include "../Modules/Transaction.h"
#include "../Modules/pch.h"
#include <QDate>
#include <QtWidgets/QMessageBox>

bool TransactionsManager::add(const Transaction& t)
{
    QSqlQuery query(db);

    query.prepare(R"(
                  INSERT INTO transactions (amount, currency, dateTime, category, account, note)
                  VALUES (:amount, :currency, :dateTime, :category, :account, :note)
    )");

    query.bindValue(":amount", t.amount);
    query.bindValue(":currency", t.currency);
    query.bindValue(":dateTime", t.dateTime.toString(Qt::ISODate));
    query.bindValue(":category", t.category);
    query.bindValue(":account", t.account);
    query.bindValue(":note", t.note);

    if (!query.exec()) { qDebug() << "Failed to execute TransactionsManager::add query"; return false; }

    return true;
}

bool TransactionsManager::clearTransactions()
{
    QSqlQuery query(db);
    return query.exec("DELETE FROM transactions");
}

QVector<DailyTransactions> TransactionsManager::transactionsPerDay(const QDate& from, const QDate& to) const
{
    QSqlQuery query(db);

    query.prepare(R"(
                  SELECT date(t.dateTime) AS day,
                  SUM(CASE WHEN t.amount < 0 THEN t.amount ELSE 0 END) AS expense,
                  SUM(CASE WHEN t.amount > 0 THEN t.amount ELSE 0 END) AS income
                  FROM transactions t
                  WHERE date(t.dateTime) BETWEEN :from AND :to
                  GROUP BY day
                  ORDER BY day ASC;
    )");

    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));

    if (!query.exec()) {
        qDebug() << "Failed to execute TransactionsManager::transactionsPerDay query";
        return {};
    }

    QVector<DailyTransactions> dailyData;
    while (query.next()) {
        DailyTransactions d;
        d.date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        d.expense = query.value(1).toDouble(); // will be negative if you store expenses as negative
        d.income = query.value(2).toDouble();
        dailyData.push_back(d);
    }

    return dailyData;
}

QVector<NamedTransactions> TransactionsManager::transactionsPerAccount(const QDate& from, const QDate& to) const
{
    QSqlQuery query(db);

    query.prepare(R"(
                  SELECT
                      a.name,
                      SUM(CASE WHEN t.amount > 0 THEN t.amount ELSE 0 END) AS positive_total,
                      SUM(CASE WHEN t.amount < 0 THEN t.amount ELSE 0 END) AS negative_total
                  FROM transactions t
                  JOIN accounts a ON a.id = t.account
                  WHERE t.dateTime BETWEEN :from AND :to
                  GROUP BY a.id, a.name
                  order by positive_total - negative_total DESC
    )");

    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));

    if (!query.exec()) {
        qDebug() << "Failed to execute TransactionsManager::transactionsPerAccount query";
        return {};
    }

    QVector<NamedTransactions> accountsData;
    while (query.next()) {
        NamedTransactions d;
        d.name = query.value(0).toString();
        d.income = query.value(1).toDouble();
        d.expense = query.value(2).toDouble();
        accountsData.push_back(d);
    }

    return accountsData;
}

QVector<QPair<QString, double>> TransactionsManager::transactionsPerCategory(const QDate& from, const QDate& to, TransactionType type) const
{
    QSqlQuery query(db);

    QString sql = R"(
                  SELECT c.name, COALESCE(SUM(t.amount), 0) as amount
                  FROM categories c
                  LEFT JOIN transactions t
                  ON c.id = t.category
                  AND date(t.dateTime) BETWEEN :from AND :to
    )";

    QString groupOrderBy = " GROUP BY c.name ORDER BY amount ";
    switch (type)
    {
    case TransactionType::Expense:
        sql += " WHERE c.isExpense = 1 " + groupOrderBy + "ASC;"; break;
    case TransactionType::Income:
        sql += " WHERE c.isExpense = 0 " + groupOrderBy + "DESC;"; break;
    case TransactionType::All:
        sql += groupOrderBy + "DESC;"; break;
    }

    query.prepare(sql);
    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));

    if (!query.exec()) { qDebug() << "Failed to execute TransactionsManager::expensePerCategory query"; return {}; }

    QVector<QPair<QString, double>> expenses;
    while (query.next()) expenses.push_back({ query.value(0).toString(), query.value(1).toDouble() });

    return expenses;
}

QVector<Transaction> TransactionsManager::get(const QDate& from, const QDate& to, int limit) const
{
    QSqlQuery query(db);

    QString sql = R"(
                  SELECT t.amount, t.currency, t.dateTime, t.category, t.account, t.note, t.id, c.name, a.name
                  FROM transactions t
                  JOIN categories c ON t.category = c.id
                  JOIN accounts a on t.account = a.id
                  WHERE date(t.dateTime) BETWEEN :from AND :to
                  ORDER BY datetime(t.dateTime) DESC
    )";
    if (limit > 0) sql.append("\nLIMIT :limit");

    query.prepare(sql);

    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));
    if (limit > 0) query.bindValue(":limit", limit);

    if (!query.exec()) { qDebug() << "Failed to execute TransactionsManager::get query"; return {}; }

    QVector<Transaction> transactions;
    while (query.next()) {
        Transaction t;
        t.amount    = query.value(0).toDouble();
        t.currency  = query.value(1).toString();
        t.dateTime  = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
        t.category  = query.value(3).toInt();
        t.account  = query.value(4).toInt();
        t.note      = query.value(5).toString();
        t.id        = query.value(6).toInt();
        t.categoryName = query.value(7).toString();
        t.accountName   = query.value(8).toString();

        transactions.push_back(std::move(t));
    }

    return transactions;
}
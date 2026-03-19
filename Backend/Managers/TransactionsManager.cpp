#include "TransactionsManager.h"
#include "../Modules/Transaction.h"
#include "../Modules/pch.h"
#include <QDate>
#include <QJsonArray>
#include <QJsonObject>
#include <QtWidgets/QMessageBox>

#include "NetworkManager.h"

bool TransactionsManager::add(const Transaction& t)
{
    return NetworkManager::blockingSqlRequest(
        QString("INSERT INTO transactions (amount, currency, dateTime, category, account, note) "
                "VALUES (%1, '%2', '%3', %4, %5, '%6')")
            .arg(t.amount).arg(t.currency).arg(t.dateTime.toString(Qt::ISODate))
            .arg(t.category).arg(t.account).arg(t.note)
    )[u"success"].toBool();
}

bool TransactionsManager::clearTransactions()
{
    return NetworkManager::blockingSqlRequest("DELETE FROM transactions")[u"success"].toBool();
}

QVector<DailyTransactions> TransactionsManager::transactionsPerDay(const QDate& from, const QDate& to) const
{
    auto res = NetworkManager::blockingSqlRequest(
        QString(R"(SELECT date(t.dateTime) AS day,
                SUM(CASE WHEN t.amount < 0 THEN t.amount ELSE 0 END) AS expense,
                SUM(CASE WHEN t.amount > 0 THEN t.amount ELSE 0 END) AS income
                FROM transactions t
                WHERE date(t.dateTime) BETWEEN '%1' AND '%2'
                GROUP BY day ORDER BY day ASC)").arg(from.toString(Qt::ISODate), to.toString(Qt::ISODate)));

    if (!res[u"success"].toBool()) return {};

    QVector<DailyTransactions> ret;
    for (const auto& row : res[u"rows"].toArray()) {
        DailyTransactions d;
        d.date    = QDate::fromString(row[u"day"].toString(), Qt::ISODate);
        d.expense = row[u"expense"].toDouble();
        d.income  = row[u"income"].toDouble();
        ret.push_back(d);
    }
    return ret;
}

QVector<NamedTransactions> TransactionsManager::transactionsPerAccount(const QDate& from, const QDate& to) const
{
    auto res = NetworkManager::blockingSqlRequest(
        QString(R"(SELECT a.name,
                SUM(CASE WHEN t.amount > 0 THEN t.amount ELSE 0 END) AS positive_total,
                SUM(CASE WHEN t.amount < 0 THEN t.amount ELSE 0 END) AS negative_total
                FROM transactions t
                JOIN accounts a ON a.id = t.account
                WHERE t.dateTime BETWEEN '%1' AND '%2'
                GROUP BY a.id, a.name
                ORDER BY positive_total - negative_total DESC)").arg(from.toString(Qt::ISODate), to.toString(Qt::ISODate)));

    if (!res[u"success"].toBool()) return {};

    QVector<NamedTransactions> ret;
    for (const auto& row : res[u"rows"].toArray()) {
        NamedTransactions d;
        d.name    = row[u"name"].toString();
        d.income  = row[u"positive_total"].toDouble();
        d.expense = row[u"negative_total"].toDouble();
        ret.push_back(d);
    }
    return ret;
}

QVector<QPair<QString, double>> TransactionsManager::transactionsPerCategory(const QDate& from, const QDate& to, TransactionType type) const
{
    QString sql = QString(R"(SELECT c.name, COALESCE(SUM(t.amount), 0) AS amount
                FROM categories c
                LEFT JOIN transactions t ON c.id = t.category
                AND date(t.dateTime) BETWEEN '%1' AND '%2')")
                    .arg(from.toString(Qt::ISODate), to.toString(Qt::ISODate));

    QString groupOrderBy = " GROUP BY c.name ORDER BY amount ";
    switch (type) {
    case TransactionType::Expense: sql += " WHERE c.isExpense = 1 " + groupOrderBy + "ASC;";  break;
    case TransactionType::Income:  sql += " WHERE c.isExpense = 0 " + groupOrderBy + "DESC;"; break;
    case TransactionType::All:     sql += groupOrderBy + "DESC;"; break;
    }

    auto res = NetworkManager::blockingSqlRequest(sql);
    if (!res[u"success"].toBool()) return {};

    QVector<QPair<QString, double>> ret;
    for (const auto& row : res[u"rows"].toArray())
        ret.push_back({ row[u"name"].toString(), row[u"amount"].toDouble() });
    return ret;
}

QVector<Transaction> TransactionsManager::get(const QDate& from, const QDate& to, int limit) const
{
    QString sql = QString(R"(SELECT t.amount, t.currency, t.dateTime, t.category, t.account, t.note, t.id,
                c.name AS categoryName, a.name AS accountName
                FROM transactions t
                JOIN categories c ON t.category = c.id
                JOIN accounts a ON t.account = a.id
                WHERE date(t.dateTime) BETWEEN '%1' AND '%2'
                ORDER BY datetime(t.dateTime) DESC)").arg(from.toString(Qt::ISODate), to.toString(Qt::ISODate));

    if (limit > 0) sql.append(QString(" LIMIT %1").arg(limit));

    auto res = NetworkManager::blockingSqlRequest(sql);
    if (!res[u"success"].toBool()) return {};

    QVector<Transaction> ret;
    for (const auto& row : res[u"rows"].toArray()) {
        Transaction t;
        t.amount       = row[u"amount"].toDouble();
        t.currency     = row[u"currency"].toString();
        t.dateTime     = QDateTime::fromString(row[u"dateTime"].toString(), Qt::ISODate);
        t.category     = row[u"category"].toInt();
        t.account      = row[u"account"].toInt();
        t.note         = row[u"note"].toString();
        t.id           = row[u"id"].toInt();
        t.categoryName = row[u"categoryName"].toString();
        t.accountName  = row[u"accountName"].toString();
        ret.push_back(std::move(t));
    }
    return ret;
}
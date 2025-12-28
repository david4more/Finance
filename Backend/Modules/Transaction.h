#pragma once

#include <QDateTime>

const QString transactionsTable = R"(
    CREATE TABLE transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    amount REAL,
    dateTime TEXT,
    note TEXT,
    currency TEXT,
    category INTEGER,
    account INTEGER,
    FOREIGN KEY(currency) REFERENCES currencies(code),
    FOREIGN KEY(category) REFERENCES categories(id),
    FOREIGN KEY(account) REFERENCES accounts(id))
)";

class Transaction
{
public:
    // recurrence
    int id;
    double amount;
    QDateTime dateTime;
    QString note;

    QString currency;
    int category = -1;
    int account = -1;
    QString categoryName;
    QString accountName;

    Transaction() = default;
    Transaction(double amount, QString currency, QDateTime dateTime, int category, QString categoryName, int account, QString accountName) :
        amount(amount), currency(currency), dateTime(dateTime), category(category), categoryName(categoryName), account(account), accountName(accountName) {}

    explicit operator bool() const {
        return !(amount == 0.f || currency.isEmpty() || dateTime.isNull() || category < 0 || account < 0);
    }
};

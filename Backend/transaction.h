#pragma once

#include <QString>
#include <QDateTime>

class Transaction
{
public:
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

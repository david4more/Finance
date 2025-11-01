#pragma once

#include <QString>
#include <QMap>

class Currency
{
    QString name, code, symbol;
    double rate;

public:
    Currency() = default;
    void init(QString n, QString c, QString s, double r) { name = n; code = c; symbol = s; rate = r; }
    Currency(QString n, QString c, QString s, double r) : name(n), code(c), symbol(s), rate(r) {}

    double convertToDefault(double amount) { return amount * rate; };
};

class Currencies
{
    QMap<QString, Currency> currencies;
    QString defaultCurrency;
public:

    void init(QString defaultCurrency) {};
    QStringList codes() const { return currencies.keys(); };
    QString getDefaultCurrency() const { return defaultCurrency; }
    void value(QString cur) { currencies[cur]; }

    static QString table() {
        return "CREATE TABLE IF NOT EXISTS currencies ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT UNIQUE NOT NULL,"
               "symbol TEXT UNIUE NOT NULL)";
    }

};

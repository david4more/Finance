#pragma once

const QString currenciesTable = R"(
    CREATE TABLE currencies (
    code TEXT PRIMARY KEY,
    rate REAL NOT NULL,
    symbol TEXT)
)";

class Currency
{
public:
    QString code;
    double rate;
    QString symbol;

    Currency(QString c, double r, QString s) : code(c), rate(r), symbol(s) {}
};
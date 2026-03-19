#pragma once

class Currency
{
public:
    QString code;
    double rate;
    QString symbol;

    Currency(QString c, double r, QString s) : code(c), rate(r), symbol(s) {}
};
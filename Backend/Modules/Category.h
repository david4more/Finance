#pragma once

class Category
{
public:
    explicit Category(int id, QString n, bool i, QString c) : name(n), color(c), isExpense(i), id(id) {}

    int id, monthlyLimit;
    QString name, color;
    bool isExpense;
};

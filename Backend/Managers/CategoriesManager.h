#pragma once

#include "../Modules/Utils.h"
#include "../Modules/Category.h"

class BACKEND_EXPORT CategoriesManager
{
    friend class Backend;
    bool setupDefault();
    bool init();
    QVector<Category> _categories;

    static const QString defaultColor;

signals:
    void categoriesUpdated();

public:
    [[nodiscard]] QMap<QString, double> getLimits() const;
    int findId(QString name, bool isExpense = true) const;
    [[nodiscard]] QVector<Category> get() const;
    [[nodiscard]] QStringList getNames(TransactionType type = TransactionType::All) const;
    bool add(QString name, bool isExpense, int monthlyLimit = 1000, QString color = defaultColor);
};

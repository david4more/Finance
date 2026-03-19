#pragma once

#include "Modules/Utils.h"
#include <QObject>
class TransactionsManager;
class CurrenciesManager;
class CategoriesManager;
class AccountsManager;

class BACKEND_EXPORT Backend : public QObject
{
        Q_OBJECT

signals:
    void firstLaunch();

public:
    Backend(QObject* parent = nullptr);
    ~Backend();

    TransactionsManager* transactions() { return _transactions; }
    CurrenciesManager* currencies() { return _currencies; }
    AccountsManager* accounts() { return _accounts; }
    CategoriesManager* categories() { return _categories; }

    bool generateTransactions();
    bool executeQuery(const QString& query);

    void waitForServer(std::function<void()> onReady);

private:

    TransactionsManager* _transactions = nullptr;
    CurrenciesManager* _currencies = nullptr;
    AccountsManager* _accounts = nullptr;
    CategoriesManager* _categories = nullptr;
};

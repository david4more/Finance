#include "Backend.h"

#include "Managers/TransactionsManager.h"
#include "Managers/CurrenciesManager.h"
#include "Managers/CategoriesManager.h"
#include "Managers/AccountsManager.h"
#include "Modules/Transaction.h"
#include "Modules/pch.h"
#include <QtDebug>
#include <QFile>
#include <QJsonObject>
#include <QTimer>

#include "Managers/NetworkManager.h"

Backend::Backend(QObject* parent) : QObject(parent),
    _transactions(new TransactionsManager), _currencies(new CurrenciesManager), _accounts(new AccountsManager), _categories(new CategoriesManager)
{}

bool Backend::generateTransactions()
{
    NetworkManager::blockingSqlRequest("BEGIN TRANSACTION");
    NetworkManager::blockingSqlRequest("DELETE FROM transactions");

    QVector<QString> currencies = _currencies->codes();
    QVector<QString> expenseCategories = _categories->getNames(TransactionType::Expense);
    QVector<QString> incomeCategories = _categories->getNames(TransactionType::Income);
    QVector<QString> accounts = _accounts->getNames();

    //QVector<int> budgets;
    //query.exec("select id from budgets");
    //while (query.next()) budgets.push_back(query.value(0).toInt());
    //query.clear();

    srand(static_cast<unsigned int>(QTime::currentTime().msec()));
    QPair expenseRange = {5, 100};
    QPair incomeRange = {20, 200};
    int transactionsNumber = 200;
    for (int i = 0; i < transactionsNumber; ++i) {
        bool isExpense = rand() % 2 == 0;
        QString category = isExpense
                               ? expenseCategories[rand() % expenseCategories.size()]
                               : incomeCategories[rand() % incomeCategories.size()];

        QString currency = currencies[rand() % currencies.size()];
        QDateTime dateTime = QDateTime::currentDateTime().addDays(-(rand() % 30));
        QString account = accounts[rand() % accounts.size()];

        float amount;
        if (isExpense)
            amount = -(expenseRange.first + rand() % expenseRange.second + (rand() % 100) / 100.0);
        else
            amount = incomeRange.first + rand() % incomeRange.second + (rand() % 100) / 100.0;


        if (!_transactions->add(std::move(Transaction(
            amount, currency, dateTime,
            _categories->findId(category, isExpense),
            category,
            _accounts->findId(account),
            account))))
            { NetworkManager::blockingSqlRequest("ROLLBACK"); return false; }
    }

    NetworkManager::blockingSqlRequest("COMMIT");
    return true;
}

bool Backend::executeQuery(const QString& query)
{
    NetworkManager::blockingSqlRequest(query);
    return true;
}

void Backend::waitForServer(std::function<void()> onReady)
{
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer, onReady]() {
        if (NetworkManager::blockingSqlRequest("PING")[u"success"].toBool()) {
            timer->stop();
            timer->deleteLater();

            _categories->init();
            _accounts->init();
            _currencies->init();

            onReady();
        } else {
            qDebug() << "Server not available, retrying...";
        }
    });
    timer->start(1000);
}

Backend::~Backend()
{
    delete _transactions;
    delete _currencies;
    delete _accounts;
    delete _categories;
}

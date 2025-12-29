#include "Backend.h"

#include "Managers/TransactionsManager.h"
#include "Managers/CurrenciesManager.h"
#include "Managers/CategoriesManager.h"
#include "Managers/AccountsManager.h"
#include "Modules/Transaction.h"
#include "Modules/Category.h"

#include <QSqlQuery>
#include <QtDebug>
#include <QFile>

void Backend::initialize()
{
    if (initialized) return;
    const QString name = "main", path = "finance.db";

    if (QSqlDatabase::contains(name)) {
        qDebug() << "not first call";
        return;
    }

    bool isFirstLaunch = !QFile::exists(path);

    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(path);
    if (!db.open()) qDebug() << "Failed to open DB";

    _transactions = new TransactionsManager(db);
    _currencies = new CurrenciesManager(db);
    _accounts = new AccountsManager(db);
    _categories = new CategoriesManager(db);

    if (isFirstLaunch) {
        createTables();
        setupDefault();
        emit firstLaunch();
    }
    else {
        _categories->init();
        _accounts->init();
        _currencies->init();
    }

    initialized = true;
}

bool Backend::createTables()
{
    QSqlQuery query(db);
    for (const auto& q : { categoriesTable, currenciesTable, accountsTable, transactionsTable })
        if (!query.exec(q)) { qDebug() << "Failed to create table"; return false; }

    return true;
}

bool Backend::setupDefault()
{
    if (_transactions == nullptr || _categories == nullptr || _currencies == nullptr || _accounts == nullptr) return false;

    bool defSetup = (_categories->setupDefault() && _currencies->setupDefault() && _accounts->setupDefault());

    _categories->init();
    _accounts->init();
    _currencies->init();

    return defSetup && generateTransactions();
}

bool Backend::generateTransactions()
{
    if (!db.transaction()) { qDebug() << "Failed to initialize a transaction"; return false; }

    QSqlQuery query(db);
    query.exec("DELETE FROM transactions");
    query.clear();

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
            return false;
    }

    if (!db.commit()) { qDebug() << "Failed to commit transaction"; return false; }
    return true;
}

bool Backend::executeQuery(const QString& query)
{
    QSqlQuery q(db);
    return q.exec(query);
}

Backend::~Backend()
{
    delete _transactions;
    delete _currencies;
    delete _accounts;
    delete _categories;
}


























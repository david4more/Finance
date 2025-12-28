#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../Backend/Backend.h"
#include "Managers/CurrenciesManager.h"
#include "Managers/CategoriesManager.h"
#include "Managers/TransactionsManager.h"
#include "Modules/Model.h"

#include "Pages/Home/HomePage.h"
#include "Pages/Transactions/TransactionsPage.h"
#include "Pages/Settings/SettingsPage.h"
#include "Pages/NewTransaction/NewTransactionForm.h"
#include "CustomFilters/CustomFiltersForm.h"

#include <QButtonGroup>
#include <QMessageBox>

#include "Managers/AccountsManager.h"


MainWindow::~MainWindow() { delete ui; }
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // initialize backend and model+proxy
    backend = new Backend(this);
    connect(backend, &Backend::firstLaunch, this, &MainWindow::onFirstLaunch);
    backend->initialize();

    model = new TransactionModel(this, backend->currencies()->rates(), backend->currencies()->symbols());
    proxy = new TransactionProxy(this);
    proxy->setSourceModel(model);

    // group MainWindow's buttons
    pages = new QButtonGroup(this);
    pages->addButton(ui->home);
    pages->addButton(ui->transactions);
    pages->addButton(ui->settings);
    pages->setExclusive(true);

    // initialize pages and connect signals to change the current one
    ui->pages->addWidget(homePage = new HomePage(ui->pages));
    ui->pages->addWidget(transactionsPage = new TransactionsPage(model, proxy, ui->pages));
    ui->pages->addWidget(settingsPage = new SettingsPage(ui->pages));
    connect(ui->home, &QToolButton::clicked, this, [this]{ changePage(Page::Home); });
    connect(ui->transactions, &QToolButton::clicked, this, [this]{ changePage(Page::Transactions); });
    connect(ui->settings, &QToolButton::clicked, this, [this]{ changePage(Page::Settings); });

    // connect pages' functionality
    setupTransactionsPageAndForm();
    setupSettingsPage();
    connect(homePage, &HomePage::requestData, this, [this] {
        auto from = QDate::currentDate().addMonths(-1);
        auto to = QDate::currentDate();

        homePage->setData(
            backend->transactions()->transactionsPerCategory(from, to, TransactionType::Expense),
            backend->categories()->getLimits(),
            backend->transactions()->transactionsPerDay(from, to),
            backend->currencies()->base());
    });

    refresh();
    changePage(Page::Home);
}

void MainWindow::setupTransactionsPageAndForm()
{
    ui->pages->addWidget(newTransactionForm = new NewTransactionForm(ui->pages));
    connect(transactionsPage, &TransactionsPage::newTransaction, this, [this]{ changePage(Page::NewTransaction); });
    connect(transactionsPage, &TransactionsPage::customFilters, this, [this] { changePage(Page::CustomFilters); });

    connect(transactionsPage, &TransactionsPage::updateTransactions, this, [this] (QPair<QDate, QDate> range)
        { model->setTransactions(backend->transactions()->get(range.first, range.second)); });

    connect(transactionsPage, &TransactionsPage::requestFilters, this, [this](TransactionType type){
        transactionsPage->setFilters(eCategories + iCategories, accounts, currencies);
    });


    connect(newTransactionForm, &NewTransactionForm::addTransaction, this, [this](Transaction t, bool isExpense){
        t.category = backend->categories()->findId(t.categoryName, isExpense);
        if (!t) return;
        backend->transactions()->add(t);
        refresh();
        changePage(Page::Transactions);
    });
    connect(newTransactionForm, &NewTransactionForm::goBack, this, [this] { changePage(Page::Transactions);} );
    connect(newTransactionForm, &NewTransactionForm::requestFilters, this, [this] {
        newTransactionForm->setFilters(eCategories, iCategories, accounts, currencies);
    });

    connect(newTransactionForm, &NewTransactionForm::addCategory, this, &MainWindow::onAddCategory);
    connect(newTransactionForm, &NewTransactionForm::addAccount, this, &MainWindow::onAddAccount);
}

void MainWindow::onAddCategory()
{

}

void MainWindow::onAddAccount()
{

}

void MainWindow::refresh()
{
    eCategories = backend->categories()->getNames(TransactionType::Expense);
    iCategories = backend->categories()->getNames(TransactionType::Income);
    accounts = backend->accounts()->getNames();
    currencies = backend->currencies()->codes();

    homePage->refresh();
    transactionsPage->refresh();
    newTransactionForm->refresh();
    if (customFiltersForm) customFiltersForm->refresh();
}

void MainWindow::setupSettingsPage()
{
    connect(settingsPage, &SettingsPage::generateTransactions, this, [this]
        { backend->generateTransactions(); refresh(); });
    connect(settingsPage, &SettingsPage::requestCurrencies, this, [this](QString currencies, QString base)
        { backend->currencies()->requestLatest(std::move(currencies), std::move(base)); refresh(); });
}

void MainWindow::changePage(Page p)
{
    switch (p) {
    case Page::NewTransaction:
        newTransactionForm->clearForm();
        break;
    case Page::CustomFilters: {
        auto* customFiltersForm = new CustomFiltersForm(proxy, this);

        connect(customFiltersForm, &CustomFiltersForm::finished, transactionsPage, &TransactionsPage::onCustomFiltersFinished);
        connect(customFiltersForm, &CustomFiltersForm::requestData, this, [this, customFiltersForm] {
            customFiltersForm->setData(eCategories, iCategories, accounts, currencies);
        });
        connect(customFiltersForm, &CustomFiltersForm::addCategory, this, &MainWindow::onAddCategory);
        connect(customFiltersForm, &CustomFiltersForm::addAccount, this, &MainWindow::onAddAccount);
        customFiltersForm->refresh();

        customFiltersForm->setAttribute(Qt::WA_DeleteOnClose);
        customFiltersForm->open();
        return;
    }
    }

    ui->pages->setCurrentIndex(static_cast<int>(p));
}

void MainWindow::onFirstLaunch()
{
    /*
    QDialog* dialog = new QDialog(this);
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QLabel* label = new QLabel("Welcome! Add a few categories to begin:", dialog);
    QVector<QLineEdit*> lines;
    lines.push_back(new QLineEdit(dialog));
    QPushButton* add = new QPushButton("Add", dialog);
    QPushButton* def = new QPushButton("Use default", dialog);
    QPushButton* done = new QPushButton("Done", dialog);
    layout->addWidget(label);
    layout->addWidget(lines.back());
    layout->addWidget(add);
    layout->addWidget(def);
    layout->addWidget(done);

    connect(add, &QPushButton::clicked, dialog, [&]{ lines.push_back(new QLineEdit(dialog)); layout->insertWidget(lines.size() - 1, lines.back()); });
    connect(done, &QPushButton::clicked, dialog, [&]{
        QStringList cats;
        for (auto l : lines)
            if (!l->text().isEmpty())
                cats << l->text();

        if (cats.isEmpty()) {
            label->setText("Unable to proceed without categories");
            return;
        }

        for (auto c : cats) backend->categories()->add(c, true);

        dialog->accept();
    });
    connect(def, &QPushButton::clicked, dialog, [&]{ backend->categories()->setupDefault(); dialog->accept(); });

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowFlag(Qt::WindowCloseButtonHint, false);
    dialog->exec();
    */

    // backend->setupDefault();
    QMessageBox::information(this, "Welcome!",
        "CoinWarden launched for the first time (since last database deletion)!");
}























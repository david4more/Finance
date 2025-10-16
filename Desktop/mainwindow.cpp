#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resourses.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    this->resize(1100, 800);
    connect(ui->homeButton, &QToolButton::clicked, this, [this]{ changePage(Page::home); });
    connect(ui->transactionsButton, &QToolButton::clicked, this, [this]{ changePage(Page::transactions); });
    connect(ui->settingsButton, &QToolButton::clicked, this, [this]{ changePage(Page::settings); });
    connect(ui->newTransactionButton, &QToolButton::clicked, this, [this]{ changePage(Page::newTransaction);});
    connect(ui->addTransactionButton, &QToolButton::clicked, this, &MainWindow::onAddTransaction);

    ui->pages->setCurrentIndex(0);
    ui->transactionsList->setModel(&model);
    updateTransactions();
    for (auto x : Res::currencies) ui->tCurrency->addItem(x);
    ui->tDateTime->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::updateTransactions()
{
    model.setTransactions(backend.getTransactions(QDate(2025,10,1), QDate(2025,11,30)));
}

void MainWindow::onAddTransaction()
{
    Transaction t;
    t.name = ui->tName->text();
    t.amount = ui->tAmount->value();
    t.currency = ui->tCurrency->currentText();
    t.dateTime = ui->tDateTime->dateTime();
    t.category = ui->tCategory->currentText();
    t.account = ui->tAccount->currentText();
    t.note = ui->tNote->text();
    backend.newTransaction(std::move(t));

    updateTransactions();
    changePage(Page::transactions);
}

void MainWindow::changePage(Page p)
{
    ui->pages->setCurrentIndex(p);
}

                        // Transaction model methods

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return {};

    const Transaction& t = transactions[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return t.name;
        case 1: return t.amount;
        case 2: return t.currency;
        case 3: return t.dateTime.toString(Qt::ISODate);
        case 4: return t.category;
        case 5: return t.account;
        case 6: return t.note;
        }
    }

    return {};
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return "Name";
        case 1: return "Amount";
        case 2: return "Currency";
        case 3: return "Date";
        case 4: return "Category";
        case 5: return "Account";
        case 6: return "Note";
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

void TransactionModel::setTransactions(QVector<Transaction>&& t)
{
    beginResetModel();
    transactions = std::move(t);
    endResetModel();
}























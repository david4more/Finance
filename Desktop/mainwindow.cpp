#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    connect(ui->newTransactionButton, &QPushButton::clicked, this, [this]{ changePage(Page::newTransaction);});
    connect(ui->customFilterButton, &QToolButton::clicked, this, [this]{ changePage(Page::customFilters);});
    connect(ui->addTransactionButton, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(ui->applyCustomFiltersButton, &QPushButton::clicked, this, &MainWindow::onApplyCustomFilters);

    connect(ui->prevMonthButton, &QToolButton::clicked, this, [&]{onMonthButton(false);});
    connect(ui->nextMonthButton, &QToolButton::clicked, this, [&]{onMonthButton(true);});
    connect(ui->noFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.enabled = false});
    });
    connect(ui->expenseFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.maxAmount = 0.f});
    });
    connect(ui->incomeFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.minAmount = 0.f});
    });

    model = new TransactionModel(ui->centralwidget);
    proxy = new TransactionProxy(ui->centralwidget);
    proxy->setSourceModel(model);
    ui->transactionsTable->setModel(proxy);

    ui->transactionsTable->resizeColumnsToContents();
    ui->transactionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->transactionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->transactionsTable->setAlternatingRowColors(true);
    ui->transactionsTable->setSortingEnabled(true);
    ui->transactionsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->transactionsTable->setItemDelegate(new TransactionDelegate(ui->centralwidget));

    ui->pages->setCurrentIndex(0);
    updateTransactions();
    for (const auto& x : backend.getCurrencies()) ui->tCurrency->addItem(x);
    for (const auto& x : backend.getAccounts()) ui->tAccount->addItem(x);
    for (const auto& x : backend.getCategories()) ui->tCategory->addItem(x);

    QButtonGroup* group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->noFilterButton);
    group->addButton(ui->expenseFilterButton);
    group->addButton(ui->incomeFilterButton);
    group->addButton(ui->customFilterButton);

}

void MainWindow::onApplyCustomFilters()
{
    ;
}

void MainWindow::onMonthButton(bool next)
{
    ;
}

void MainWindow::updateTransactions()
{
    model->setTransactions(backend.getTransactions(QDate(2025,10,1), QDate(2025,11,30)));
}

void MainWindow::onAddTransaction()
{
    Transaction t;
    t.amount = ui->tAmount->value();
    t.currency = ui->tCurrency->currentText();
    t.dateTime = ui->tDateTime->dateTime();
    t.category = ui->tCategory->currentText();
    t.account = ui->tAccount->currentText();
    t.note = ui->tNote->text();

    if (!t) {
        highlightField(ui->tAmount, ui->tAmount->value() == 0.f);
        return;
    }

    backend.newTransaction(std::move(t));

    updateTransactions();
    changePage(Page::transactions);
}

void MainWindow::highlightField(QWidget* widget, bool condition)
{
    if (!condition) return;

    QTimer* timer = widget->findChild<QTimer*>("errorTimer");
    if (!timer) {
        timer = new QTimer(widget);
        timer->setObjectName("errorTimer");
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, [widget]() {
            if (widget) widget->setStyleSheet("");
        });
    }

    widget->setStyleSheet("background-color: #5a1f1f;");
    timer->stop();
    timer->start(2000);
}

void MainWindow::clearTransactionForm()
{
    ui->tAmount->setValue(0);
    ui->tCurrency->setCurrentIndex(0);
    ui->tDateTime->setDateTime(QDateTime::currentDateTime());
    ui->tCategory->setCurrentIndex(0);
    ui->tAccount->setCurrentIndex(0);
    ui->tNote->clear();
}

void MainWindow::changePage(Page p)
{
    ui->pages->setCurrentIndex(p);

    if (p == Page::newTransaction) clearTransactionForm();
}























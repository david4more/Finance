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
    connect(ui->categoryFilterButton, &QToolButton::clicked, this, &MainWindow::onCategoryFilterButton);

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

    changePage(Page::home);
    start = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    finish = start.addMonths(1);
    updateTransactions();
    updateData();

    QButtonGroup* group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->noFilterButton);
    group->addButton(ui->expenseFilterButton);
    group->addButton(ui->incomeFilterButton);
    group->addButton(ui->categoryFilterButton);
    group->addButton(ui->customFilterButton);
}

void MainWindow::onMonthButton(bool next)
{
    int v = next ? 1 : -1;
    start = start.addMonths(v);
    finish = finish.addMonths(v);
    updateTransactions();
}

void MainWindow::onAddCategory()
{

}

void MainWindow::onApplyCustomFilters()
{
    ;
}

void MainWindow::updateTransactions()
{
    ui->monthLabel->setText(start.toString("MMMM yyyy"));
    model->setTransactions(backend.getTransactions(start, finish));
}

void MainWindow::onCategoryFilterButton()
{
    QDialog* dialog = new QDialog(this);
    QVBoxLayout* layout = new QVBoxLayout(dialog);

    for (const auto& item : backend.getCategories()) {
        QCheckBox* box = new QCheckBox(item, dialog);
        box->setChecked(pickedCategories.contains(item));
        layout->addWidget(box);
    }

    QPushButton* button = new QPushButton("Done", dialog);
    layout->addWidget(button);

    // Update pickedCategories whenever a checkbox is clicked
    auto updateCategoriesFilter = [dialog, this]{
        pickedCategories.clear();
        for (auto* box : dialog->findChildren<QCheckBox*>()) {
            if (box->checkState() == Qt::Checked)
                pickedCategories.append(box->text());
        }

        proxy->useFilters({.categories = pickedCategories});
    };

    for (auto* box : dialog->findChildren<QCheckBox*>()) {
        connect(box, &QCheckBox::clicked, dialog, updateCategoriesFilter);
    }

    connect(button, &QPushButton::clicked, dialog, [dialog]{ dialog->accept(); });

    connect(dialog, &QDialog::finished, this, [=](int){
        if (pickedCategories.empty())
            ui->noFilterButton->click();
        else
            updateCategoriesFilter();
    });

    updateCategoriesFilter();
    dialog->show();
}

void MainWindow::updateData()
{
    auto updateCombo = [&](QComboBox* box, QStringList list) {
        box->clear();
        for (const auto& x : list) box->addItem(x);
    };
    updateCombo(ui->tCurrency, backend.getCurrencies());
    updateCombo(ui->tAccount, backend.getAccounts());
    updateCombo(ui->tCategory, backend.getCategories());
}

void MainWindow::onAddTransaction()
{
    Transaction t;
    t.amount = ui->tAmount->value();
    t.currency = ui->tCurrency->currentText();
    t.dateTime = QDateTime(ui->tDate->date(), ui->tTime->time());
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
    ui->tDate->setDate(QDate::currentDate());
    ui->tTime->setTime(QTime::currentTime());
    ui->tCategory->setCurrentIndex(0);
    ui->tAccount->setCurrentIndex(0);
    ui->tNote->clear();
}

void MainWindow::changePage(Page p)
{
    ui->pages->setCurrentIndex(p);

    if (p == Page::newTransaction) clearTransactionForm();
}























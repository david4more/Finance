#include "TransactionsPage.h"
#include "ui_TransactionsPage.h"

#include "../Dialogs/MultiSelect/MultiSelectDialog.h"
#include "../../../Backend/Modules/Model.h"
#include "../../../Backend/Modules/Utils.h"

#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>

class TransactionDelegate : public QStyledItemDelegate
{
public:
    explicit TransactionDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt(option);

        opt.state &= ~QStyle::State_MouseOver;
        opt.font.setFamily("Cascadia Mono");

        QStyledItemDelegate::paint(painter, opt, index);
    }
};

TransactionsPage::~TransactionsPage() { delete ui; }

TransactionsPage::TransactionsPage(TransactionModel* model, TransactionProxy* proxy, QWidget* parent) :
    QWidget(parent), ui(new Ui::TransactionsPage), model(model), proxy(proxy)
{
    ui->setupUi(this);

    connect(ui->prevMonth, &QToolButton::clicked, this, [&]{ onMonthButton(false); });
    connect(ui->nextMonth, &QToolButton::clicked, this, [&]{ onMonthButton(true); });
    connect(ui->date, &QToolButton::clicked, this, &TransactionsPage::onCustomMonth);
    connect(ui->newTransaction, &QPushButton::clicked, this, [this]{ emit newTransaction(); });
    connect(ui->customFilter, &QPushButton::clicked, this, [this]{
        ui->categoryFilter->setCurrentIndex(0); ui->accountFilter->setCurrentIndex(0); ui->currencyFilter->setCurrentIndex(0);
        ui->noFilter->click(); emit customFilters();
    });

    ui->transactionsTable->resizeColumnsToContents();
    ui->transactionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->transactionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->transactionsTable->setAlternatingRowColors(true);
    ui->transactionsTable->setSortingEnabled(true);
    ui->transactionsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->transactionsTable->setItemDelegate(new TransactionDelegate(this));
    ui->transactionsTable->setModel(proxy);
    ui->transactionsTable->setColumnHidden(5, true);

    month = QDate::currentDate().month();
    year = QDate::currentDate().year();
    emit updateTransactions(getDateRange());

    types = new QButtonGroup(this);
    types->setExclusive(true);
    types->addButton(ui->noFilter, 0);
    types->addButton(ui->expenseFilter, 1);
    types->addButton(ui->incomeFilter, 2);
    connect(types, &QButtonGroup::idClicked, this, &TransactionsPage::onTypeClicked);

    auto connectCombo = [this](MyComboBox* combo, Filter type) {
        connect(combo, &QComboBox::activated, this, [type, combo, this] (int index)
            { onComboFilter(combo, type); });
    };
    connectCombo(ui->categoryFilter, Filter::Category);
    connectCombo(ui->accountFilter, Filter::Account);
    connectCombo(ui->currencyFilter, Filter::Currency);

    connect(ui->categoryFilter, &MyComboBox::aboutToShowPopup, this, [this]
        { if (types->checkedId() == 0) types->button(1)->click(); });
}

void TransactionsPage::refresh()
{
    auto range = getDateRange();
    ui->date->setText(range.first.toString("MMMM yyyy"));
    emit updateTransactions(range);
    emit requestFilters(type);
    updateFilters();
}

void TransactionsPage::setFilters(QStringList categories, QStringList accounts, QStringList currencies)
{
    this->categories = std::move(categories);
    this->accounts = std::move(accounts);
    this->currencies = std::move(currencies);
}

void TransactionsPage::updateFilters()
{
    auto updateCombo = [this](QComboBox* combo, const QStringList& items) {
        combo->clear();
        combo->addItem("All");
        combo->addItem("Multiple...");
        combo->addItems(items);
    };
    updateCombo(ui->categoryFilter, this->categories);
    updateCombo(ui->accountFilter, this->accounts);
    updateCombo(ui->currencyFilter, this->currencies);
}

void TransactionsPage::onCustomFiltersFinished(int result)
{
    if (result != QDialog::Accepted)
        ui->noFilter->click();
}

void TransactionsPage::onComboFilter(QComboBox* combo, Filter filter)
{
    if (combo->currentIndex() == 0) {
        auto filters = proxy->getFilters();
        switch (filter) {
            case Filter::Category: filters.categories.reset(); break;
            case Filter::Account: filters.accounts.reset(); break;
            case Filter::Currency: filters.currencies.reset(); break;
        }
        proxy->setFilters(filters);
        return;
    }

    QStringList items;
    if (combo->currentIndex() == 1) {
        QString title = "Pick ";
        QStringList list;
        switch (filter) {
        case Filter::Category: list = categories; title += "categories"; break;
        case Filter::Account: list = accounts; title += "accounts"; break;
        case Filter::Currency: list = currencies; title += "currencies"; break;
        }

        items = MultiSelectDialog::getSelectedItems(
            std::move(title),
            list,
            this);

        if (items.empty()) { combo->setCurrentIndex(0); return; }
    }
    else
        items += combo->currentText();

    TransactionProxy::Filters filters = proxy->getFilters();
    filters.isExpense = types->checkedId() == 1;

    switch (filter) {
    case Filter::Category: filters.categories = items; break;
    case Filter::Account: filters.accounts = items; break;
    case Filter::Currency: filters.currencies = items; break;
    }
    proxy->setFilters(filters);
}

void TransactionsPage::onTypeClicked(int index)
{
    type = static_cast<TransactionType>(index);

    if (ui->customFilter->isChecked()) {
        proxy->resetFilters();
        ui->customFilter->setChecked(false);
    }

    auto filters = proxy->getFilters();
    switch (type) {
    case TransactionType::All:
        filters = {}; break;
    case TransactionType::Expense:
        filters.isExpense = true; filters.categories.reset(); break;
    case TransactionType::Income:
        filters.isExpense = false; filters.categories.reset(); break;
    }
    proxy->setFilters(filters);

    if (type != TransactionType::All)
        emit requestFilters(type);

    ui->categoryFilter->setCurrentIndex(0);
}

void TransactionsPage::onCustomMonth()
{
    QDialog* dialog = new QDialog(this);

    dialog->setLayout(new QVBoxLayout(dialog));
    dialog->setWindowTitle("Select month");

    QLabel* monthLabel = new QLabel("Month", dialog);
    QSpinBox* month = new QSpinBox(dialog);
    month->setRange(1, 12);
    month->setValue(this->month);

    QLabel* yearLabel = new QLabel("Year", dialog);
    QSpinBox* year = new QSpinBox(dialog);
    year->setRange(1926, 2126);
    year->setValue(this->year);

    QPushButton* button = new QPushButton("Done", dialog);

    dialog->layout()->addWidget(monthLabel);
    dialog->layout()->addWidget(month);
    dialog->layout()->addWidget(yearLabel);
    dialog->layout()->addWidget(year);
    dialog->layout()->addWidget(button);

    connect(button, &QPushButton::clicked, dialog, [dialog, month, year, this] {
        this->month = month->value(); this->year = year->value();

        emit updateTransactions(getDateRange()); dialog->accept();
    });

    dialog->adjustSize();
    dialog->exec();
}

void TransactionsPage::onMonthButton(bool next)
{
    if (next) {
        if (month == 12) { month = 1; year++; }
        else month++;
    } else {
        if (month == 1) { month = 12; year--; }
        else month--;
    }

    refresh();
}

QPair<QDate, QDate> TransactionsPage::getDateRange() const
{
    if (month < 1 || month > 12) {
        auto from = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
        auto to = QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().daysInMonth());

        return {from, to};
    };

    int daysInMonth = QDate(year, month, 1).daysInMonth();
    return {QDate(year, month, 1), QDate(year, month, daysInMonth)};
}







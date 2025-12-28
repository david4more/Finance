#include "NewTransactionForm.h"
#include "ui_NewTransactionForm.h"

#include "../../Backend/Modules/Transaction.h"
#include "../Utils.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QPushButton>

#include "Utils.h"

NewTransactionForm::~NewTransactionForm() { delete ui; }

NewTransactionForm::NewTransactionForm(QWidget* parent) :
    QWidget(parent), ui(new Ui::NewTransactionForm)
{
    ui->setupUi(this);

    connect(ui->add, &QToolButton::clicked, this, &NewTransactionForm::onAddTransaction);
    connect(ui->reset, &QToolButton::clicked, this, [this]
        { if (QMessageBox::question(this, "Confirmation", "Are you sure you want to clear the form?") == QMessageBox::Yes) clearForm(); });
    connect(ui->back, &QToolButton::clicked, this, [this]{ emit goBack(); });

    auto* group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->expense, 1);
    group->addButton(ui->income, 2);

    connect(group, &QButtonGroup::idClicked, this, &NewTransactionForm::updateFilters);

    connect(ui->addCategory, &QPushButton::clicked, this, [this]{ emit addCategory(); });
    connect(ui->addAccount, &QPushButton::clicked, this, [this]{ emit addAccount(); });
}

void NewTransactionForm::refresh()
{
    emit requestFilters();
    updateFilters();
}

void NewTransactionForm::setFilters(QStringList eCategories, QStringList iCategories, QStringList accounts, QStringList currencies)
{
    this->eCategories = std::move(eCategories);
    this->iCategories = std::move(iCategories);
    this->accounts = std::move(accounts);
    this->currencies = std::move(currencies);
}

void NewTransactionForm::updateFilters(int index)
{
    auto updateCombo = [&](QComboBox* box, const QStringList& list) {
        box->clear();
        for (const auto& x : list) box->addItem(x);
    };
    updateCombo(ui->category, ui->expense->isChecked() ? eCategories : iCategories);
    updateCombo(ui->account, accounts);
    updateCombo(ui->currency, currencies);
}

void NewTransactionForm::clearForm()
{
    ui->amount->setValue(0);
    ui->currency->setCurrentIndex(0);
    ui->dateTime->setDateTime(QDateTime::currentDateTime());
    ui->category->setCurrentIndex(0);
    ui->account->setCurrentIndex(0);
    ui->note->clear();
}

void NewTransactionForm::onAddTransaction()
{
    if (ui->amount->value() == 0.f) {
        Utils::highlightField(ui->amount);
        return;
    }

    Transaction t;
    t.amount = (ui->income->isChecked()) ? ui->amount->value() : -ui->amount->value();
    t.currency = ui->currency->currentText();
    t.dateTime = QDateTime(ui->dateTime->dateTime());
    t.categoryName = ui->category->currentText();
    t.accountName = ui->account->currentText();
    t.note = ui->note->text();

    emit addTransaction(std::move(t), ui->expense->isChecked());
}

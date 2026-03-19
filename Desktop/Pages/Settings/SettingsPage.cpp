#include "SettingsPage.h"
#include "ui_SettingsPage.h"

#include <QPushButton>
#include <QMessageBox>


SettingsPage::SettingsPage(QWidget* parent) :
    QWidget(parent), ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    this->resize(1000, 800);

    connect(ui->resetTransactions, &QPushButton::clicked, this, &SettingsPage::onResetTransactions);
    connect(ui->getCurrencies, &QPushButton::clicked, this, &SettingsPage::onGetCurrencies);
    connect(ui->clearTransactions, &QPushButton::clicked, this, [this] {
        if (QMessageBox::question(this, "Confirmation", "Are you sure you want to clear all transactions?") == QMessageBox::Yes)
            emit clearTransactions();
    });
    connect(ui->executeQuery, &QPushButton::clicked, this, [this]{ emit executeQuery(ui->query->text()); });
}

void SettingsPage::onGetCurrencies()
{
    //if (ui->currencies->text().isEmpty() || ui->base->text().isEmpty()) { QMessageBox::information(this, "Error", "Fields are empty"); return; }

    emit requestCurrenciesUpdated();
    if (currenciesUpdated) { QMessageBox::information(this, "Failure", "Exchange rates were already up to date"); return; }
    emit requestCurrencies("EUR,GBP,CHF,PLN,UAH,USD,CAD,JPY,CNY", "EUR");
    QMessageBox::information(this, "Success", "Exchange rates are updated");
}

void SettingsPage::onResetTransactions()
{
    if (QMessageBox::question(this,
        "Confirmation", "Are you sure you want to reset all transactions?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    emit generateTransactions();
}

SettingsPage::~SettingsPage()
{
    delete ui;
}
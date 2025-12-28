#pragma once

#include <QWidget>
#include "Transaction.h"
class Backend;

namespace Ui { class NewTransactionForm; }

class NewTransactionForm : public QWidget
{
    Q_OBJECT

public:
    explicit NewTransactionForm(QWidget* parent = nullptr);
    ~NewTransactionForm() override;

    void refresh();
    void clearForm();
    void setFilters(QStringList eCategories, QStringList iCategories, QStringList accounts, QStringList currencies);

signals:
    void goBack();
    void addTransaction(Transaction t, bool isExpense);
    void requestFilters();
    void addCategory();
    void addAccount();

private:
    QStringList eCategories, iCategories, accounts, currencies;
    Ui::NewTransactionForm* ui;

    void onAddTransaction();
    void updateFilters(int index = -1);
};

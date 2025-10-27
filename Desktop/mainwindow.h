#pragma once

#include "../Backend/backend.h"
#include "table.h"
#include <QMainWindow>
#include <QTimer>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum Page { home, transactions, settings, newTransaction, customFilters };

    Ui::MainWindow *ui;
    Backend backend;
    TransactionModel* model;
    TransactionProxy* proxy;

    void setupUI();

    // slots
    void onMonthButton(bool next);
    void onAddTransaction();
    void onApplyCustomFilters();

    // helpers
    void updateTransactions();

    void changePage(Page p);
    void highlightField(QWidget* widget, bool condition);
    void clearTransactionForm();
};

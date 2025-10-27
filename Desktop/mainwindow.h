#pragma once

#include "../Backend/backend.h"
#include "../Backend/model.h"
#include <QMainWindow>
#include <QTimer>
#include <QButtonGroup>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class TransactionDelegate : public QStyledItemDelegate
{
public:
    explicit TransactionDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionViewItem opt(option);
        opt.state &= ~QStyle::State_MouseOver;
        QStyledItemDelegate::paint(painter, opt, index);
    }
};

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

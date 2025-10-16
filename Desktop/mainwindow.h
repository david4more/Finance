#pragma once

#include "../Backend/backend.h"
#include <QMainWindow>
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT
    QVector<Transaction> transactions;
public:
    void setTransactions(QVector<Transaction>&& t);
    int rowCount(const QModelIndex&) const override { return transactions.size(); }
    int columnCount(const QModelIndex&) const override { return Transaction::fields; }
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum Page { home, transactions, settings, newTransaction };

    Ui::MainWindow *ui;
    Backend backend;
    TransactionModel model;

    void setupUI();

    // slots
    void updateTransactions();
    void onAddTransaction();

    // helpers
    void changePage(Page p);
};

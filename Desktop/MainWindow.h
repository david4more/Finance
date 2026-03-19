#pragma once

#include <QMainWindow>
#include <QDate>
#include <QStringList>

#include "Pages/Utils.h"

namespace Ui { class MainWindow; }

class HomePage;
class TransactionsPage;
class SettingsPage;
class NewTransactionForm;
class Backend;
class QButtonGroup;
class TransactionModel;
class TransactionProxy;
class CustomFiltersForm;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStringList eCategories, iCategories, accounts, currencies;

    Ui::MainWindow *ui;
    Backend* backend;

    QButtonGroup* pages;
    HomePage* homePage;
    TransactionsPage* transactionsPage;
    SettingsPage* settingsPage;
    NewTransactionForm* newTransactionForm;
    CustomFiltersForm* customFiltersForm = nullptr;

    TransactionModel* model;
    TransactionProxy* proxy;

    void setupUi();

    void onAddCategory();
    void onAddAccount();
    void refresh();
    void onFirstLaunch();
    void changePage(Page p);

    void setupTransactionsPageAndForm();
    void setupSettingsPage();
};
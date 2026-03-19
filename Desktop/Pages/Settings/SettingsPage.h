#pragma once

#include <QWidget>

namespace Ui { class SettingsPage; }

class SettingsPage : public QWidget
{
    Q_OBJECT

    bool currenciesUpdated = false;
public:
    void setCurrenciesUpdated(bool updated) { currenciesUpdated = updated; }
    explicit SettingsPage(QWidget* parent = nullptr);
    ~SettingsPage() override;

signals:
    void requestCurrenciesUpdated();
    void executeQuery(QString query);
    void clearTransactions();
    void generateTransactions();
    void requestCurrencies(QString currencies, QString base);

private:
    Ui::SettingsPage* ui;

    void onResetTransactions();
    void onGetCurrencies();
};
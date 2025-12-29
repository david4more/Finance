#pragma once

#include <QWidget>

namespace Ui { class SettingsPage; }

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget* parent = nullptr);
    ~SettingsPage() override;

signals:
    void executeQuery(QString query);
    void clearTransactions();
    void generateTransactions();
    void requestCurrencies(QString currencies, QString base);

private:
    Ui::SettingsPage* ui;

    void onResetTransactions();
    void onGetCurrencies();
};
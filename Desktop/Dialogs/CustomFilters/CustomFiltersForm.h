#pragma once

#include <QDialog>
class TransactionProxy;
class QCheckBox;

namespace Ui { class CustomFiltersForm; }

class CustomFiltersForm : public QDialog
{
    Q_OBJECT

public:
    explicit CustomFiltersForm(TransactionProxy* proxy, QWidget* parent = nullptr);
    ~CustomFiltersForm() override;

    void refresh();
    void setData(QStringList eCategories, QStringList iCategories, QStringList accounts, QStringList currencies);

signals:
    void requestData();
    void addCategory();
    void addAccount();

private:
    QStringList eCategories, iCategories, accounts, currencies;

    Ui::CustomFiltersForm* ui;
    TransactionProxy* proxy;

    template <typename T>
    void connectFilter(auto&& widget, auto&& signal, QCheckBox* checkBox, std::optional<T>& filter);
    void updateData();
    void onButtonGroupClicked(int index);
};



#pragma once
#include <QDialog>

namespace Ui { class MultiSelectDialog; }

class QStandardItemModel;
class MultiSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiSelectDialog(QString title, QStringList list, QWidget* parent = nullptr);
    ~MultiSelectDialog() override;
    static QStringList getSelectedItems(QString title, QStringList list, QWidget* parent = nullptr);

private:
    QStringList acceptedItems;
    Ui::MultiSelectDialog* ui;
    QStandardItemModel* model;

    void onDone();
};
#include "MultiSelectDialog.h"
#include "ui_MultiSelectDialog.h"

#include <QStandardItemModel>

MultiSelectDialog::~MultiSelectDialog() { delete ui; }
MultiSelectDialog::MultiSelectDialog(QString title, QStringList list, QWidget* parent) :
    QDialog(parent), ui(new Ui::MultiSelectDialog)
{
    ui->setupUi(this);

    ui->title->setText(title);
    model = new QStandardItemModel(this);
    for (const auto& text : list) {
        auto* item = new QStandardItem(text);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);
        model->appendRow(item);
    }
    ui->list->setModel(model);

    connect(ui->done, &QPushButton::clicked, this, &MultiSelectDialog::onDone);
    connect(ui->cancel, &QPushButton::clicked, this, &QDialog::reject);
}


QStringList MultiSelectDialog::getSelectedItems(QString title, QStringList list, QWidget* parent)
{
    MultiSelectDialog dialog(title, list, parent);

    if (dialog.exec() == QDialog::Accepted)
        return dialog.acceptedItems;

    return {};
}

void MultiSelectDialog::onDone()
{
    for (int i = 0; i < model->rowCount(); i++) {
        if (model->item(i)->checkState() == Qt::Checked)
            acceptedItems.append(model->item(i)->text());
    }

    this->accept();
}

#pragma once
#include <QDialog>


namespace Ui { class NewCategoryDialog; }
class NewCategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewCategoryDialog(QWidget* parent = nullptr);
    ~NewCategoryDialog() override;

private:
    Ui::NewCategoryDialog* ui;
};
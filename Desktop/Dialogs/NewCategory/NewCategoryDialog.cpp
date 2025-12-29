//
// Created by david on 29.12.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NewCategoryDialog.h" resolved

#include "NewCategoryDialog.h"
#include "ui_NewCategoryDialog.h"


NewCategoryDialog::NewCategoryDialog(QWidget* parent) :
    QDialog(parent), ui(new Ui::NewCategoryDialog)
{
    ui->setupUi(this);
}

NewCategoryDialog::~NewCategoryDialog()
{
    delete ui;
}
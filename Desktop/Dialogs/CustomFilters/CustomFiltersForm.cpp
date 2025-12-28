#include "CustomFiltersForm.h"
#include "ui_CustomFiltersForm.h"

#include "../../Backend/Modules/Model.h"

#include <QButtonGroup>
#include <utility>

CustomFiltersForm::~CustomFiltersForm() { delete ui; }

CustomFiltersForm::CustomFiltersForm(TransactionProxy* proxy, QWidget* parent) :
    QDialog(parent), ui(new Ui::CustomFiltersForm), proxy(proxy)
{
    ui->setupUi(this);

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->expense);
    buttonGroup->addButton(ui->income);
    buttonGroup->setExclusive(true);

    ui->from->setDateTime(QDateTime::currentDateTime().addMonths(-1));
    ui->to->setDateTime(QDateTime::currentDateTime());

    proxy->resetFilters();
    onButtonGroupClicked(0);

    connect(buttonGroup, &QButtonGroup::idClicked, this, &CustomFiltersForm::onButtonGroupClicked);
    connect(ui->apply, &QPushButton::clicked, this, [this] { this->accept(); });
    connect(ui->cancel, &QPushButton::clicked, this, [this] { this->reject(); });


    auto& filters = proxy->mutableFilters();
    connectFilter(ui->limit, &QDoubleSpinBox::valueChanged, ui->limitBox, filters.maxAmount);
    connectFilter(ui->from, &QDateEdit::dateChanged, ui->fromBox, filters.from);
    connectFilter(ui->to, &QDateEdit::dateChanged, ui->toBox, filters.to);
    connectFilter(ui->note, &QLineEdit::textChanged, ui->noteBox, filters.note);

    // TODO: implement multi options selection
    connectFilter(ui->category, &QComboBox::currentTextChanged, ui->categoryBox, filters.categories);
    connectFilter(ui->currency, &QComboBox::currentTextChanged, ui->currencyBox, filters.currencies);
    connectFilter(ui->account, &QComboBox::currentTextChanged, ui->accountBox, filters.accounts);
}

template <typename T>
void CustomFiltersForm::connectFilter(auto&& widget, auto&& signal, QCheckBox* checkBox, std::optional<T>& filter)
{
    auto widgetValue = [](QWidget* widget) -> T {
        if (auto combo = qobject_cast<QComboBox*>(widget)) {
            if constexpr (std::is_same_v<T, QStringList>) {
                return QStringList{combo->currentText()};
            }
        }
        if (auto line = qobject_cast<QLineEdit*>(widget)) {
            if constexpr (std::is_same_v<T, QString>) {
                return line->text();
            }
        }
        if (auto spin = qobject_cast<QDoubleSpinBox*>(widget)) {
            if constexpr (std::is_same_v<T, double>) {
                return spin->value();
            }
        }
        if (auto date = qobject_cast<QDateEdit*>(widget)) {
            if constexpr (std::is_same_v<T, QDate>) {
                return date->date();
            }
        }

        return T{};
    };

    connect(widget, signal, this, [this, &filter, checkBox, widget, widgetValue](auto&& v) {
        if (!checkBox->isChecked()) return;

        filter = widgetValue(widget);
        proxy->invalidate();
    });
    connect(checkBox, &QCheckBox::toggled, this, [this, &filter, widget, widgetValue](bool checked) {
        if (checked)
            filter = widgetValue(widget);
        else
            filter = std::nullopt;
        proxy->invalidate();
    });
}

void CustomFiltersForm::onButtonGroupClicked(int index)
{
    ui->category->clear();
    ui->category->addItems(ui->expense->isChecked() ? eCategories : iCategories);
    proxy->mutableFilters().isExpense = ui->expense->isChecked();
    proxy->invalidate();
}

void CustomFiltersForm::refresh()
{
    emit requestData();
    updateData();
}

void CustomFiltersForm::setData(QStringList eCategories, QStringList iCategories, QStringList accounts,
                                QStringList currencies)
{
    this->eCategories = std::move(eCategories);
    this->iCategories = std::move(iCategories);
    this->accounts = std::move(accounts);
    this->currencies = std::move(currencies);
}

void CustomFiltersForm::updateData()
{
    ui->category->clear();
    ui->category->addItems(ui->expense->isChecked() ? eCategories : iCategories);

    ui->account->clear();
    ui->account->addItems(accounts);

    ui->currency->clear();
    ui->currency->addItems(currencies);
}

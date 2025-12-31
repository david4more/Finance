#include "Model.h"


// Transaction proxy methods

void TransactionProxy::setFilters(Filters f)
{
    filters = std::move(f);
    invalidate();
}

bool TransactionProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!filters) return true;

    auto data = [this, &sourceRow, &sourceParent](int column) { return sourceModel()->index(sourceRow, column, sourceParent).data(Qt::UserRole); };

    if (filters->isExpense || filters->maxAmount) {
        double amt = data(0).toDouble();
        if (filters->isExpense && ((amt < 0) != *filters->isExpense)) return false;
        if (filters->maxAmount && std::abs(amt) > *filters->maxAmount) return false;
    }

    if (filters->categories && !filters->categories->contains(
        data(2).toString())) return false;

    if (filters->accounts && !filters->accounts->contains(
        data(3).toString())) return false;

    if (filters->currencies && !filters->currencies->contains(
        data(5).toString())) return false;

    if (filters->from || filters->to) {
        QDate date  = data(1).toDate();
        if (filters->from && date < filters->from) return false;
        if (filters->to && date > filters->to) return false;
    }

    if (filters->note &&
        !data(4).toString().contains(*filters->note)) return false;

    return true;
}


// Transaction model methods

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return {};

    const Transaction& t = transactions[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return QString("%1%2%3")
            .arg(symbols[t.currency])
            .arg(t.amount < 0 ? "" : " ")
            .arg(QString::number(t.amount, 'f', 2));
        case 1: return t.dateTime.toString("MMM dd, hh:mm");
        case 2: return t.categoryName;
        case 3: return t.accountName;
        case 4: return t.note.isEmpty() ? "None" : t.note;
        }
    }
    else if (role == Qt::UserRole) {
        switch (index.column()) {
        case 0: return t.amount / currencies[t.currency];
        case 1: return t.dateTime;
        case 2: return t.categoryName;
        case 3: return t.accountName;
        case 4: return t.note;
        case 5: return t.currency;
        }
    }
    // else if (role == Qt::TextAlignmentRole && index.column() == 0) return QVariant(int(Qt::AlignRight | Qt::AlignVCenter));

    return {};
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return "Amount";
        case 1: return "Date";
        case 2: return "Category";
        case 3: return "Account";
        case 4: return "Note";
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

void TransactionModel::setTransactions(QVector<Transaction>&& t)
{
    beginResetModel();
    transactions = std::move(t);
    endResetModel();
}
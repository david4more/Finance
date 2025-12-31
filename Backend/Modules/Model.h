#pragma once

#include "Utils.h"
#include <QSortFilterProxyModel>
#include "Transaction.h"

class Transaction;
class BACKEND_EXPORT TransactionModel : public QAbstractTableModel
{
    Q_OBJECT
    QVector<Transaction> transactions;
    QMap<QString, double> currencies;
    QMap<QString, QChar> symbols;
public:
    explicit TransactionModel(QObject* parent, QMap<QString, double> currencies, QMap<QString, QChar> symbols)
    : QAbstractTableModel(parent), currencies(currencies), symbols(symbols) {}

    void setTransactions(QVector<Transaction>&& t);
    int rowCount(const QModelIndex&) const override { return transactions.size(); }
    int columnCount(const QModelIndex&) const override { return 6; }
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};


class BACKEND_EXPORT TransactionProxy : public QSortFilterProxyModel
{
public:
    struct Filters {
        std::optional<bool> isExpense;
        std::optional<double> maxAmount;
        std::optional<QStringList> currencies;
        std::optional<QStringList> categories;
        std::optional<QStringList> accounts;
        std::optional<QDate> from, to;
        std::optional<QString> note;
    };
    explicit TransactionProxy(QObject* parent) : QSortFilterProxyModel(parent) { setSortRole(Qt::UserRole); setFilterRole(Qt::UserRole); }

    void setFilters(Filters f);
    void resetFilters() { filters.reset(); invalidate(); }
    [[nodiscard]] Filters getFilters() const { if (filters) return *filters; else return {}; }
    Filters& mutableFilters() { if (!filters) filters.emplace(); return *filters; }

private:
    std::optional<Filters> filters;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};


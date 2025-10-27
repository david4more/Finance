#pragma once

#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QPainter>
#include "backend.h"

class TransactionDelegate : public QStyledItemDelegate
{
public:
    explicit TransactionDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT
    QVector<Transaction> transactions;
public:
    explicit TransactionModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}

    void setTransactions(QVector<Transaction>&& t);
    int rowCount(const QModelIndex&) const override { return transactions.size(); }
    int columnCount(const QModelIndex&) const override { return 5; }
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

class TransactionProxy : public QSortFilterProxyModel {
    QStringList categories = {};
    float minAmount = 0.f, maxAmount = 0.f;
    bool useCategoryFilter = false, useMinFilter = false, useMaxFilter = false;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
public:
    explicit TransactionProxy(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}

    struct Filters {
        bool enabled = true;
        std::optional<float> minAmount;
        std::optional<float> maxAmount;
        std::optional<QStringList> categories;
    };
    void useFilters(Filters f);
};

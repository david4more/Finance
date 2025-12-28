#pragma once

#include <QWidget>
#include <QVector>
#include <../Backend/Modules/Utils.h>
class QCustomPlot;
class QCPBars;

namespace Ui { class HomePage; }

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage() override;
    void refresh();
    void setData(QVector<QPair<QString, double>> t, QMap<QString, double> l, QString base, QVector<DailyTransactions> d, QVector<NamedTransactions> a);

signals:
    void requestData();

private:
    QCPBars* expenseBar;
    QCPBars* limitBar;
    Ui::HomePage* ui;

    QVector<QPair<QString, double>> transactionsData;
    QMap<QString, double> limitsData;
    QVector<DailyTransactions> dailyData;
    QVector<NamedTransactions> accountsData;
    QString baseCurrency;

    void updateAccountsList();
    void updateFinancesIndicator();
    void updateFinancesData();
    void updateCategoriesData();
    void setupFinancesPlot();
    void setupCategoriesPlot();
    QVector<double> smoothGraph(const QVector<double>& data, const QVector<double>& x);
};

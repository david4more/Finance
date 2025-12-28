#include "HomePage.h"
#include "ui_HomePage.h"
#include "../QCustomPlot/qcustomplot.h"

#include <QDateTime>

HomePage::~HomePage() { delete ui; }

void HomePage::refresh()
{
    emit requestData();

    updateFinancesData();
    updateCategoriesData();
    updateFinancesIndicator();
    updateAccountsList();
}

void HomePage::updateAccountsList()
{
    ui->accountsList->clear();

    for (const auto& a : accountsData) {
        auto nameItem = new QListWidgetItem(a.name);
        auto expenseItem = new QListWidgetItem(QString::number(a.expense, 'f', 2) + " ");
        auto incomeItem = new QListWidgetItem(QString::number(a.income, 'f', 2) + " ");

        expenseItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        incomeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->accountsList->addItem(nameItem);
        ui->accountsList->addItem(expenseItem);
        ui->accountsList->addItem(incomeItem);
    }
}

void HomePage::updateFinancesIndicator()
{
    double expenseSum = 0, incomeSum = 0;
    for (int i = 0; i < dailyData.size(); ++i) {
        incomeSum += dailyData[i].income;
        expenseSum += dailyData[i].expense;
    }
    auto incomeLayout = qobject_cast<QVBoxLayout*>(ui->incomeLine->parentWidget()->layout());
    auto expenseLayout = qobject_cast<QVBoxLayout*>(ui->expenseLine->parentWidget()->layout());
    if (incomeSum > expenseSum) {
        incomeLayout->setStretch(0, 0);
        incomeLayout->setStretch(1, 1);

        double ratio = -expenseSum / incomeSum;
        int ratioFirst = ratio * 100;
        int ratioSecond = 100 - ratioFirst;

        expenseLayout->setStretch(0, ratioFirst);
        expenseLayout->setStretch(1, ratioSecond);
    }
    else {
        expenseLayout->setStretch(0, 1);
        expenseLayout->setStretch(1, 0);

        double ratio = incomeSum / -expenseSum;
        int ratioFirst = ratio * 100;
        int ratioSecond = 100 - ratioFirst;

        incomeLayout->setStretch(0, ratioFirst);
        incomeLayout->setStretch(1, ratioSecond);
    }
}

void HomePage::setData(QVector<QPair<QString, double>> t, QMap<QString, double> l, QString base, QVector<DailyTransactions> d, QVector<NamedTransactions> a)
{
    transactionsData = std::move(t);
    limitsData = std::move(l);
    baseCurrency = std::move(base);
    dailyData = std::move(d);
    accountsData = std::move(a);
}

HomePage::HomePage(QWidget* parent) :
    QWidget(parent), ui(new Ui::HomePage)
{
    ui->setupUi(this);

    setupFinancesPlot();
    setupCategoriesPlot();
}

void HomePage::setupCategoriesPlot()
{
    auto* plot = ui->categoriesPlot;

    plot->setBackground(QBrush(QColor(30, 30, 30)));

    expenseBar = new QCPBars(plot->xAxis, plot->yAxis);
    expenseBar->setPen(QPen(QColor(Qt::red).darker(150), 2));
    expenseBar->setBrush(QBrush(QColor(Qt::red)));

    limitBar = new QCPBars(plot->xAxis, plot->yAxis);
    limitBar->setPen(QPen(QColor(Qt::white).darker(75), 2));
    limitBar->setBrush(QBrush(QColor(Qt::transparent)));

    plot->xAxis->setBasePen(QPen(Qt::white));
    plot->xAxis->setTickPen(QPen(Qt::white));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));

    plot->yAxis->setBasePen(QPen(Qt::white));
    plot->yAxis->setTickPen(QPen(Qt::white));
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);
    plot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));

    plot->xAxis->setTickLength(0, 4);
}

void HomePage::updateCategoriesData()
{
    auto* plot = ui->categoriesPlot;

    QVector<double> ticks;
    QVector<double> values, limits;
    QVector<QString> labels;
    if (transactionsData.empty()) return;

    for (int i = 0; i < transactionsData.size(); ++i) {
        ticks << i + 1;
        values << -transactionsData[i].second;

        auto label = transactionsData[i].first;
        limits << limitsData[label];
        if (label.size() > 10) label = label.left(6) + ".";
        labels << label;
    }

    expenseBar->setData(ticks, values);
    limitBar->setData(ticks, limits);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    plot->xAxis->setTicker(textTicker);
    plot->xAxis->setRange(0, transactionsData.size() + 1);

    plot->yAxis->setRange(values[0] * -0.05f, values[0] * 1.05f);
    ui->categoriesPlotLabel->setText("Expense per category in " + baseCurrency);

    plot->replot();
}

void HomePage::setupFinancesPlot()
{
    auto* plot = ui->financesPlot;

    plot->setBackground(QBrush(QColor(30, 30, 30)));

    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::green));
    plot->graph(0)->setBrush(QBrush(QColor(0, 255, 0, 20)));
    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(1)->setBrush(QBrush(QColor(255, 0, 0, 20)));

    plot->xAxis2->setVisible(true);
    plot->xAxis2->setTickLabels(false);
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setTickLabels(false);

    plot->xAxis->setBasePen(QPen(Qt::white));
    plot->xAxis->setTickPen(QPen(Qt::white));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));

    plot->yAxis->setBasePen(QPen(Qt::white));
    plot->yAxis->setTickPen(QPen(Qt::white));
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);
    plot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
}

void HomePage::updateFinancesData()
{
    auto* plot = ui->financesPlot;

    QVector<double> x(dailyData.size()), y0(dailyData.size()), y1(dailyData.size());
    if (dailyData.size() == 0) return;
    for (int i = 0; i < dailyData.size(); ++i)
    {
        x[i] = i - dailyData.size() + 1;
        y0[i] = dailyData[i].income;
        y1[i] = dailyData[i].expense;
    }

    plot->graph(0)->setData(x, y0);
    plot->graph(1)->setData(x, y1);


    int size = dailyData.size() - 1;
    double min = *std::min_element(y1.begin(), y1.end());
    double max = *std::max_element(y0.begin(), y0.end());
    plot->xAxis->setRange(size * -1.01, size * 0.01);
    plot->yAxis->setRange(min * 1.1, max * 1.1);

    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTick(0, "Today");
    ticker->addTick(-7, "A week ago");
    ticker->addTick(-14, "Two weeks ago");
    ticker->addTick(-21, "Three weeks ago");
    ticker->addTick(-size, "A month ago");
    ui->financesPlot->xAxis->setTicker(ticker);

    plot->replot();
}

QVector<double> HomePage::smoothGraph(const QVector<double>& data, const QVector<double>& x)
{
    if (data.isEmpty() || data.size() > x.size()) return {};

    QVector<double> smoothed(data.size());

    int smoothness = data.size() / 40;
    int half = smoothness / 2;

    for (int i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        int count = 0;

        // accumulate values in a window
        for (int j = -half; j <= half; ++j) {
            int idx = i + j;
            if (idx >= 0 && idx < data.size()) {
                sum += data[idx];
                ++count;
            }
        }

        smoothed[i] = sum / count;
    }

    return smoothed;
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    connect(ui->homeButton, &QToolButton::clicked, this, [this]{ changePage(Page::home); });
    connect(ui->transactionsButton, &QToolButton::clicked, this, [this]{ changePage(Page::transactions); });
    connect(ui->settingsButton, &QToolButton::clicked, this, [this]{ changePage(Page::settings); });
    connect(ui->newTransactionButton, &QPushButton::clicked, this, [this]{ changePage(Page::newTransaction);});
    connect(ui->customFilterButton, &QToolButton::clicked, this, [this]{ changePage(Page::customFilters);});
    connect(ui->addTransactionButton, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(ui->applyCustomFiltersButton, &QPushButton::clicked, this, &MainWindow::onApplyCustomFilters);

    connect(ui->prevMonthButton, &QToolButton::clicked, this, [&]{onMonthButton(false);});
    connect(ui->nextMonthButton, &QToolButton::clicked, this, [&]{onMonthButton(true);});
    connect(ui->noFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.enabled = false});
    });
    connect(ui->expenseFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.maxAmount = 0.f});
    });
    connect(ui->incomeFilterButton, &QToolButton::clicked, this, [&]{
        proxy->useFilters({.minAmount = 0.f});
    });
    connect(ui->categoryFilterButton, &QToolButton::clicked, this, &MainWindow::onCategoryFilterButton);

    backend = new Backend(this);
    connect(backend, &Backend::message, this, [this](QString newStatus){ QMessageBox::critical(this, "Error", newStatus); });
    backend->init();

    model = new TransactionModel(ui->centralwidget);
    proxy = new TransactionProxy(ui->centralwidget);
    proxy->setSourceModel(model);
    ui->transactionsTable->setModel(proxy);

    ui->transactionsTable->resizeColumnsToContents();
    ui->transactionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->transactionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->transactionsTable->setAlternatingRowColors(true);
    ui->transactionsTable->setSortingEnabled(true);
    ui->transactionsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->transactionsTable->setItemDelegate(new TransactionDelegate(ui->centralwidget));

    changePage(Page::home);
    start = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    finish = start.addMonths(1);
    updateTransactions();
    updateData();

    QButtonGroup* group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->noFilterButton);
    group->addButton(ui->expenseFilterButton);
    group->addButton(ui->incomeFilterButton);
    group->addButton(ui->categoryFilterButton);
    group->addButton(ui->customFilterButton);

    setupFinancesPlot(ui->financesPlot);
    setupCategoriesPlot(ui->categoriesPlot);
}

void MainWindow::setupFinancesPlot(QCustomPlot *plot)
{
    // add two new graphs and set their look:
    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(251), y0(251), y1(251);
    for (int i=0; i<251; ++i)
    {
        x[i] = i;
        y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
        y1[i] = qExp(-i/150.0);              // exponential envelope
    }
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    plot->xAxis2->setVisible(true);
    plot->xAxis2->setTickLabels(false);
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    plot->graph(0)->setData(x, y0);
    plot->graph(1)->setData(x, y1);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    plot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    plot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::setupCategoriesPlot(QCustomPlot *plot)
{
    plot->setBackground(QBrush(QColor(30, 30, 30)));

    // create empty bar chart objects:
    QCPBars *expenseBar = new QCPBars(plot->xAxis, plot->yAxis);

    expenseBar->setPen(QPen(QColor(Qt::red).darker(150), 2));
    expenseBar->setBrush(QBrush(QColor(Qt::red)));

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    auto data = backend->getCategories();
    for (int i = 0; i < data.size(); ++i) {
        ticks << i + 1;
        labels << data[i];
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    plot->xAxis->setTicker(textTicker);
    plot->xAxis->setTickLabelRotation(30);
    plot->xAxis->setSubTicks(false);
    plot->xAxis->setTickLength(0, 4);
    plot->xAxis->setRange(0, 8);
    plot->xAxis->setBasePen(QPen(Qt::white));
    plot->xAxis->setTickPen(QPen(Qt::white));
    plot->xAxis->grid()->setVisible(true);
    plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    plot->yAxis->setRange(0, 12.1);
    plot->yAxis->setPadding(5); // a bit more space to the left border
    plot->yAxis->setLabel("Expense per category\nin " + backend->defCurrency());
    plot->yAxis->setBasePen(QPen(Qt::white));
    plot->yAxis->setTickPen(QPen(Qt::white));
    plot->yAxis->setSubTickPen(QPen(Qt::white));
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);
    plot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    plot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    // Add data:
    QVector<double> expenseData;
    expenseData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2;
    expenseBar->setData(ticks, expenseData);

    plot->replot();
}


void MainWindow::onMonthButton(bool next)
{
    int v = next ? 1 : -1;
    start = start.addMonths(v);
    finish = finish.addMonths(v);
    updateTransactions();
}

void MainWindow::onAddCategory()
{
    ;
}

void MainWindow::onApplyCustomFilters()
{
    ;
}

void MainWindow::updateTransactions()
{
    ui->monthButton->setText(start.toString("MMMM yyyy"));
    model->setTransactions(backend->getTransactions(start, finish));
}

void MainWindow::onCategoryFilterButton()
{
    QDialog* dialog = new QDialog(this);
    QVBoxLayout* layout = new QVBoxLayout(dialog);

    for (const auto& item : backend->getCategories()) {
        QCheckBox* box = new QCheckBox(item, dialog);
        box->setChecked(pickedCategories.contains(item));
        layout->addWidget(box);
    }

    QPushButton* button = new QPushButton("Done", dialog);
    layout->addWidget(button);

    // Update pickedCategories whenever a checkbox is clicked
    auto updateCategoriesFilter = [dialog, this]{
        pickedCategories.clear();
        for (auto* box : dialog->findChildren<QCheckBox*>()) {
            if (box->checkState() == Qt::Checked)
                pickedCategories.append(box->text());
        }

        proxy->useFilters({.categories = pickedCategories});
    };

    for (auto* box : dialog->findChildren<QCheckBox*>()) {
        connect(box, &QCheckBox::clicked, dialog, updateCategoriesFilter);
    }

    connect(button, &QPushButton::clicked, dialog, [dialog]{ dialog->accept(); });

    connect(dialog, &QDialog::finished, this, [=](int result){
        if ((result == QDialog::Accepted && pickedCategories.empty()) || result == QDialog::Rejected)
            ui->noFilterButton->click();
    });

    updateCategoriesFilter();
    dialog->setWindowModality(Qt::WindowModal);
    dialog->show();
}

void MainWindow::updateData()
{
    auto updateCombo = [&](QComboBox* box, QStringList list) {
        box->clear();
        for (const auto& x : list) box->addItem(x);
    };
    updateCombo(ui->tCurrency, backend->getCurrencies());
    updateCombo(ui->tAccount, backend->getAccounts());
    updateCombo(ui->tCategory, backend->getCategories());
}

void MainWindow::onAddTransaction()
{
    Transaction t;
    t.amount = ui->tAmount->value();
    t.currency = ui->tCurrency->currentText();
    t.dateTime = QDateTime(ui->tDate->date(), ui->tTime->time());
    t.category = ui->tCategory->currentText();
    t.account = ui->tAccount->currentText();
    t.note = ui->tNote->text();

    if (!t) {
        highlightField(ui->tAmount, ui->tAmount->value() == 0.f);
        return;
    }

    backend->newTransaction(std::move(t));

    updateTransactions();
    changePage(Page::transactions);
}

void MainWindow::highlightField(QWidget* widget, bool condition)
{
    if (!condition) return;

    QTimer* timer = widget->findChild<QTimer*>("errorTimer");
    if (!timer) {
        timer = new QTimer(widget);
        timer->setObjectName("errorTimer");
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, [widget]() {
            if (widget) widget->setStyleSheet("");
        });
    }

    widget->setStyleSheet("background-color: #5a1f1f;");
    timer->stop();
    timer->start(2000);
}

void MainWindow::clearTransactionForm()
{
    ui->tAmount->setValue(0);
    ui->tCurrency->setCurrentIndex(0);
    ui->tDate->setDate(QDate::currentDate());
    ui->tTime->setTime(QTime::currentTime());
    ui->tCategory->setCurrentIndex(0);
    ui->tAccount->setCurrentIndex(0);
    ui->tNote->clear();
}

void MainWindow::changePage(Page p)
{
    ui->pages->setCurrentIndex(p);

    if (p == Page::newTransaction) clearTransactionForm();
}























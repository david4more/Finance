#pragma once
#include <QDateTime>
#include <QtSql/QSqlDatabase>

class QTcpServer;
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QJsonObject executeQuery(QString query);
    QJsonObject requestExchangeRates(QString currencies, QString base);

    bool createTables();
    bool setupDefaultCurrencies();
    bool setupDefaultCategories();
    bool setupDefaultAccounts();

    QString baseCurrency, apiKey;
    QDateTime lastUpdate;
    bool updated = false;

    QString _baseCurrency;
    static QString categoriesTable, currenciesTable, accountsTable, transactionsTable;

    QTcpServer* tcpServer;
    QSqlDatabase db;
};
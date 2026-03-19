#pragma once
#include <QWidget>
#include <QtSql/QSqlDatabase>

class QTcpServer;
namespace Ui { class MainWindow; }
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    //void onNewConnection();

private:
    Ui::MainWindow* ui;
    QTcpServer* server;
    QSqlDatabase db;
};
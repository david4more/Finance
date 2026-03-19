#include "mainwindow.h"
#include "ui_MainWindow.h"

#include <QtHttpServer/QtHttpServer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

MainWindow::MainWindow(QWidget* parent) :
    QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("finance.db");
    db.open();

    auto* httpServer = new QHttpServer(this);

    httpServer->route("/query", QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest& request) {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            QString sql = doc.object()["sql"].toString();

            QSqlQuery query(db);
            QJsonObject response;

            if (!query.exec(sql)) {
                response["success"] = false;
                response["error"] = query.lastError().text();
                return QHttpServerResponse(QJsonDocument(response).toJson());
            }

            QJsonArray rows;
            while (query.next()) {
                QJsonObject row;
                for (int i = 0; i < query.record().count(); i++)
                    row[query.record().fieldName(i)] = query.value(i).toJsonValue();
                rows.append(row);
            }

            response["success"] = true;
            response["rows"] = rows;
            response["affected"] = query.numRowsAffected();
            return QHttpServerResponse(QJsonDocument(response).toJson());
        });

    httpServer->listen(QHostAddress::Any, 8080);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewConnection() {}
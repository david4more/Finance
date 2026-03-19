#include "NetworkManager.h"
#include <QNetworkReply>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QTimer>

QNetworkAccessManager* NetworkManager::mgr = nullptr;

QJsonObject NetworkManager::parseJsonResponse(QByteArray data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) return {};
    return doc.object();
}

QJsonObject NetworkManager::blockingSqlRequest(const QString& query)
{
    QNetworkRequest request(QUrl("http://localhost:8080/query"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");


    QByteArray body = QJsonDocument({ { "sql", query } }).toJson();
    return blockingPostRequest(request, body, 5000);
}


QJsonObject NetworkManager::blockingGetRequest(const QNetworkRequest& request, int timeoutMs)
{
    if (!mgr) mgr = new QNetworkAccessManager;

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    QNetworkReply* reply = mgr->get(request);

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (!timer.isActive()) { qDebug() << "Request took too long - " << timeoutMs << " ms.";
        reply->abort(); reply->deleteLater(); return {}; }
    timer.stop();

    if (reply->error() != QNetworkReply::NoError) { qDebug() << reply->errorString(); reply->deleteLater(); return {}; }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    return parseJsonResponse(data);
}

QJsonObject NetworkManager::blockingPostRequest(const QNetworkRequest& request, const QByteArray& body, int timeoutMs)
{
    if (!mgr) mgr = new QNetworkAccessManager;

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    QNetworkReply* reply = mgr->post(request, body);

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (!timer.isActive()) { qDebug() << "Request took too long - " << timeoutMs << " ms.";
        reply->abort(); reply->deleteLater(); return {}; }
    timer.stop();

    if (reply->error() != QNetworkReply::NoError) { qDebug() << reply->errorString(); reply->deleteLater(); return {}; }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    return parseJsonResponse(data);
}

void NetworkManager::asyncGetRequest(const QNetworkRequest& request, std::function<void(QJsonObject)> callback)
{
    if (!mgr) mgr = new QNetworkAccessManager;

    QNetworkReply* reply = mgr->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [reply, callback]{
        if (reply->error() != QNetworkReply::NoError) { qDebug() << reply->errorString(); callback({}); return; }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        callback(parseJsonResponse(data));
    });
}

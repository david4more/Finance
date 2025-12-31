#include "NetworkManager.h"
#include <QNetworkReply>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>

QNetworkAccessManager* NetworkManager::mgr = nullptr;

QJsonObject NetworkManager::parseJsonResponse(QByteArray data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) return {};
    return doc.object();
}

QJsonObject NetworkManager::blockingNetworkRequest(const QNetworkRequest& request, int timeoutMs)
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

void NetworkManager::asyncNetworkRequest(const QNetworkRequest& request, std::function<void(QJsonObject)> callback)
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

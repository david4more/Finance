#pragma once
#include <QJsonObject>
class QNetworkAccessManager;
class QNetworkRequest;

class NetworkManager
{
    static QNetworkAccessManager* mgr;
    static QJsonObject parseJsonResponse(QByteArray data);

public:
    static QJsonObject blockingSqlRequest(const QString& query);
    [[nodiscard]] static QJsonObject blockingGetRequest(const QNetworkRequest& request, int timeoutMs = 5000);
    [[nodiscard]] static QJsonObject blockingPostRequest(const QNetworkRequest& request, const QByteArray& body, int timeoutMs);
    static void asyncGetRequest(const QNetworkRequest& request, std::function<void(QJsonObject)> callback);
};

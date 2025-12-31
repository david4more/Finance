#pragma once
class QNetworkAccessManager;
class QNetworkRequest;

class NetworkManager
{
    static QNetworkAccessManager* mgr;
    static QJsonObject parseJsonResponse(QByteArray data);

public:
    [[nodiscard]] static QJsonObject blockingNetworkRequest(const QNetworkRequest& request, int timeoutMs = 5000);
    static void asyncNetworkRequest(const QNetworkRequest& request, std::function<void(QJsonObject)> callback);
};

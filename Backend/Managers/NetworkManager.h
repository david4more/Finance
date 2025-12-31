#pragma once
class QNetworkAccessManager;
class QNetworkRequest;

class NetworkManager
{
    static QNetworkAccessManager* mgr;
    static QJsonObject parseJsonResponse(QByteArray data);

public:
    [[nodiscard]] QJsonObject blockingNetworkRequest(const QNetworkRequest& request, int timeoutMs = 5000);
    void asyncNetworkRequest(const QNetworkRequest& request, std::function<void(QJsonObject)> callback);
};

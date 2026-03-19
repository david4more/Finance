#pragma once
#include "../Modules/Utils.h"
#include "../Modules/Currency.h"
class QSqlDatabase;

class BACKEND_EXPORT CurrenciesManager
{
    friend class Backend;
    bool setupDefault();
    bool init();

    QVector<Currency> _currencies;
    QString _base;
    QSqlDatabase& db;
    QString _apiKey;
    QDateTime _lastUpdate;
    bool updated = false;

public:
    CurrenciesManager(QSqlDatabase& db) : db(db) {}

    bool requestLatest(QString currencies, QString base);
    [[nodiscard]] QMap<QString, double> rates() const;
    [[nodiscard]] QMap<QString, QChar> symbols() const;
    QStringList codes() const;
    QString base() const;
    double value(QString key) const { return rates().value(key, 0.0); }
    bool currenciesUpdated() const { return updated; }
};


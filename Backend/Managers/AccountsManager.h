#pragma once

#include "../Modules/Utils.h"
class QSqlDatabase;

const QString accountsTable = R"(
    CREATE TABLE accounts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL)
)";

class Account
{
    int id;
    QString name;
};

class BACKEND_EXPORT AccountsManager
{
    friend class Backend;
    bool setupDefault();
    bool init();

    QSqlDatabase& db;
    QStringList names;

public:
    int findId(QString name) const;
    explicit AccountsManager(QSqlDatabase& db) : db(db) {}
    [[nodiscard]] QStringList getNames() const { return names; }
    bool add(QString name);
};

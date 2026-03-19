#pragma once

#include "../Modules/Utils.h"

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

    QStringList names;

public:
    int findId(QString name) const;
    [[nodiscard]] QStringList getNames() const { return names; }
    bool add(QString name);
};

#include "backend.h"

Backend::Backend()
{
    if (!QSqlDatabase::contains("main")) {
        db = QSqlDatabase::addDatabase("QSQLITE", "main");
        db.setDatabaseName(dbName);
    } else
        db = QSqlDatabase::database("main");

    if (!db.open()) {
        qDebug() << "Failed to open DB: " << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    if (!query.exec(Transaction::table())) {
        qDebug() << "Failed to create table: " << query.lastError().text();
        return;
    }

    qDebug() << "DB ready to listen.";
}

void Backend::newTransaction(Transaction t)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO transactions (name, amount, currency, dateTime, category, account, note) "
        "VALUES (:name, :amount, :currency, :dateTime, :category, :account, :note)"
        );

    query.bindValue(":name", t.name);
    query.bindValue(":amount", t.amount);
    query.bindValue(":currency", t.currency);
    query.bindValue(":dateTime", t.dateTime);
    query.bindValue(":category", t.category);
    query.bindValue(":account", t.account);
    query.bindValue(":note", t.note);

    if (!query.exec())
        throw std::runtime_error("Failed to execute query: " + query.lastError().text().toStdString());
    else
        qDebug() << "Query executed.";
}

QVector<Transaction> Backend::getTransactions(const QDate& from, const QDate& to)
{
    QVector<Transaction> transactions;

    QSqlQuery query(db);

    // SQLite date comparison works with 'YYYY-MM-DD' strings
    query.prepare("SELECT * "
                  "FROM transactions "
                  "WHERE date(dateTime) BETWEEN :from AND :to "
                  "ORDER BY date(dateTime) ASC");

    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));


    if (!query.exec()) {
        throw std::runtime_error("Failed to execute query: " + query.lastError().text().toStdString());
        return transactions;
    }


    while (query.next()) {
        Transaction t;
        t.name      = query.value("name").toString();
        t.amount    = query.value("amount").toDouble();
        t.currency  = query.value("currency").toString();
        t.dateTime  = QDateTime::fromString(query.value("dateTime").toString(), Qt::ISODate);
        t.category  = query.value("category").toString();
        t.account   = query.value("account").toString();
        t.note      = query.value("note").toString();
        t.id        = query.value("id").toInt();

        transactions.push_back(std::move(t));
    }

    return transactions;
}



























#pragma once


#ifdef BACKEND_LIB
#  define BACKEND_EXPORT __declspec(dllexport)
#else
#  define BACKEND_EXPORT __declspec(dllimport)
#endif

#include <QDate>

enum class TransactionType { All, Expense, Income };

struct DailyTransactions { QDate date; double expense; double income; };
struct NamedTransactions { QString name; double expense; double income; };

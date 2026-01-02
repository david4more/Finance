#pragma once

#include "Export.h"

#include <QDate>

enum class TransactionType { All, Expense, Income };

struct DailyTransactions { QDate date; double expense; double income; };
struct NamedTransactions { QString name; double expense; double income; };

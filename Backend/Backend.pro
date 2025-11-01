TEMPLATE = lib
CONFIG += staticlib c++17
TARGET = Backend

QT += core sql network

SOURCES += backend.cpp \
    model.cpp
HEADERS += backend.h \
    currency.h \
    model.h \
    transaction.h

TEMPLATE = lib
CONFIG += staticlib c++17
TARGET = Backend

QT = sql

SOURCES += backend.cpp \
    model.cpp
HEADERS += backend.h \
    model.h \
    transaction.h

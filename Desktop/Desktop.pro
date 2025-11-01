QT       += core gui sql widgets network
CONFIG += c++17

SOURCES += main.cpp mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui

# Backend setup
INCLUDEPATH += $$PWD/../Backend
LIBS += -L$$OUT_PWD/../Backend/debug -lBackend

# QCustomPlot setup
INCLUDEPATH += $$PWD/QCustomPlot
LIBS += -L$$PWD/../Desktop/QCustomPlot -lqcustomplotd2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

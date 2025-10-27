QT       += core gui sql widgets
CONFIG += c++17

SOURCES += main.cpp mainwindow.cpp \
    table.cpp
HEADERS += mainwindow.h \
    table.h
FORMS += mainwindow.ui

INCLUDEPATH += $$PWD/../Backend

LIBS += -L$$OUT_PWD/../Backend/debug -lBackend

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

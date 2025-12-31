#include "mainwindow.h"

#include <QApplication>

#include <iostream>

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{ std::cout << msg.toStdString() << std::endl; }

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);

    QApplication a(argc, argv);

    QFont font("Segoe UI", 11);
    qApp->setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}
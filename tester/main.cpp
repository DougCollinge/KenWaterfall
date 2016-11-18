#include <QApplication>
#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QDebug>
#include <QtCore/QTranslator>
#include <QtCore/QMessageLogger>
#include <QtCore/QMessageLogContext>
#include <QtCore/QtMessageHandler>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QApplication>

#include "utils.hpp"
#include "ardopmodem.hpp"
#include "testpanel.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion("0.5.0a");

    QT_MESSAGELOGCONTEXT
    qInstallMessageHandler(debugMessageOutput);
    qInfo() << "WFTest Ver:" << a.applicationVersion();

    ARDOPModem *m;
    TestPanel w(m);
    w.show();

    return a.exec();
}

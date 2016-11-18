#include <QtCore/QDebug>
#include <QtCore/QTranslator>
#include <QtCore/QMessageLogger>
#include <QtCore/QMessageLogContext>
#include <QtCore/QtMessageHandler>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QSettings>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "ardop_global.hpp"
#include "ardopmodem.hpp"



int main(int argc, char *argv[])
{
//    qDebug() << "starting application";
    QApplication a(argc, argv);
//    qDebug() << "setting local app info";
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName(APP_NAME);
//    qDebug() << "installing translator";
    QTranslator translator;
    a.installTranslator(&translator);
//    qDebug() << "installing message handler";

//    qDebug() << "Starting Modem";
    ARDOPModem m;

    return a.exec();
}

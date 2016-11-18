#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QIODevice>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QTextStream>
#include <QtCore/QMessageLogger>
#include <QtCore/QMessageLogContext>
#include <QtWidgets/QApplication>

#include "ardop_global.hpp"

#include "modem.hpp"

// -------------------------------------
// log file
// -------------------------------------
void debugMessageOutput(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    bool    dbg2file = false;
    QDir    dir;
    QFile   logfile;
    QString txt = "";
    QString slogfile = "";
    QString logname = "";
    QDateTime dttTimestamp = QDateTime::currentDateTimeUtc();

    dbg2file = false;
    logname  = QString(dir.canonicalPath().toLatin1());
    logname = QDir::fromNativeSeparators(logname);
    logname += "/../Logs/ARDOP_TNC_";
    logname += dttTimestamp.toString("yyyyMMdd");
    logname += ".log";
    logname = QDir::fromNativeSeparators(logname);

    QSettings settings(logname.toLatin1(),QSettings::IniFormat);
    settings.beginGroup("CONFIG");
     dbg2file = settings.value("Dbg2File",true).toBool();
     slogfile  = settings.value("DbgFile").toString().trimmed();
    settings.endGroup();
    if ( slogfile == "" )
    {
        // do nothing
    }
    else
    {
        logname = "";
        logname = slogfile;
    }
    qDebug() << "Log file using " << logname;
    // Switch structure left to be converted to write
    // into the file in the future
    QString debugdate = QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy hh:mm:ss.zzz t ");
    QString message(msg);
    txt += debugdate + " ";
    // extract the priority of the message
    int priority = message.left(1).toInt();
    if (priority >= 0) message.remove(0, 1);

    switch ( type )
    {
    case QtInfoMsg:
        txt += "INFO [";
        txt += QString(context.function);
        txt += ":";
        txt += QString("%1").arg(context.line);
        txt += "]";
        txt += " -- ";
        txt += QString(" %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += "WARNING [";
        txt += QString(context.function);
        txt += ":";
        txt += QString("%1").arg(context.line);
        txt += "]";
        txt += " -- ";
        txt += QString(" %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += "!!!CRITICAL!!! [";
        txt += QString(context.function);
        txt += ":";
        txt += QString("%1").arg(context.line);
        txt += "]";
        txt += "--";
        txt += QString(" %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += "<<<Fatal>>> [";
        txt += QString(context.function);
        txt += ":";
        txt += QString("%1").arg(context.line);
        txt += "]";
        txt += " -- ";
        txt += QString(" %1").arg(msg);
        txt += "\n";
        txt += "%{backtrace [depth=7] [separator='...']}";
        break;
    case QtDebugMsg:
        txt += "DEBUG [";
        txt += QString(context.function);
        txt += ":";
        txt += QString("%1").arg(context.line);
        txt += "]";
        txt += "--";
        txt += QString(" %1").arg(msg);
        break;
    }

    if ( dbg2file )
    {
        if ( logname == "" )
        {
            qDebug() << "Opening logger file " << logname;
            QFile logfile(logname.toLatin1());
            bool ok = logfile.open(QIODevice::WriteOnly|QIODevice::Append);
            if ( !ok )
            {
                qDebug() << "Error on open " << logfile.errorString();
                qDebug() << logfile.errorString();
            }
        }
        else
        {
            qDebug() << "Writing to logger file " << logname;
            QFile logfile(logname.toLatin1());
            bool ok = logfile.open(QIODevice::WriteOnly|QIODevice::Append);
            if ( !ok )
            {
                qDebug() << "Error on open " << logfile.errorString();
                qDebug() << logfile.errorString();
            }
            else
            {
                qDebug() << "Opening Logfile " << logfile.fileName();
                logfile.open(QIODevice::WriteOnly | QIODevice::Append);
            }
            QTextStream ts(&logfile);
            ts << txt << endl;
            logfile.close();
        }
    }
}



int main(int argc, char *argv[])
{
    qDebug() << "starting application";
    QApplication a(argc, argv);

    qDebug() << "setting local app info";
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName(APP_NAME);

    qDebug() << "installing translator";
    QTranslator translator;
    a.installTranslator(&translator);

    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("TNC");
      bool logit = settings.value("DebugLog",false).toBool();
    settings.endGroup();
    if ( logit )
    {
      qDebug() << "installing custom message handler";
      QT_MESSAGELOGCONTEXT
      qInstallMessageHandler(debugMessageOutput);
    }
#ifdef DEBUG_MODEM
    qDebug() << "Starting MODEM";
#endif
    ARDOPModem m;
    m.setNickName("ARDOPMODEM");

    int ret = a.exec();
    qInstallMessageHandler(0);
    return ret;
}

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>

#include "tcptest.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QNetworkConfigurationManager manager;
    if ( manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired )
    {
        // Get saved network configuration
        QDir dir;
        QSettings settings(dir.canonicalPath() + "/ardop_tnc.ini",QSettings::IniFormat);
        settings.beginGroup(QLatin1String("NETWORK"));
          const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ( (config.state() & QNetworkConfiguration::Discovered) !=
              QNetworkConfiguration::Discovered
           )
        {
            config = manager.defaultConfiguration();
        }

        QNetworkSession *networkSession = new QNetworkSession(config, &a);
        networkSession->open();
        networkSession->waitForOpened();

        if ( networkSession->isOpen() )
        {
            qDebug() << "Network Session opened";
            // Save the used configuration
            QNetworkConfiguration config = networkSession->configuration();
            QString id;
            if (config.type() == QNetworkConfiguration::UserChoice)
            {
                id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
            }
            else
            {
                id = config.identifier();
            }

            settings.beginGroup(QLatin1String("NETWORK"));
              settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
            settings.endGroup();
            settings.sync();
            qDebug() << "Network Session ID saved";
        }
    }

    TcpTest w;
    w.show();

    return a.exec();
}

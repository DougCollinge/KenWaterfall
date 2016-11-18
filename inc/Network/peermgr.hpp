#ifndef PEERMGR_HPP
#define PEERMGR_HPP
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#include "tcpclient.hpp"
#include "tcpconnection.hpp"

class TCPClient;

class PeerManager : public QObject
{
    Q_OBJECT

public:
    PeerManager(TCPClient *tcpclient=0);

    void setServerPort(int port);
    QByteArray userName() const;
    void startBroadcasting();
    bool isLocalHostAddress(const QHostAddress &address);

signals:
    void newConnection(TCPConnection *connection);

private slots:
    void sendBroadcastDatagram();
    void readBroadcastDatagram();
    void slotNewConnection(TCPConnection *connection);

private:
    int serverPort;

    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;
    QList<QString>      macaddresses;

    QUdpSocket broadcastSocket;

    QTimer broadcastTimer;

    QByteArray username;

    TCPClient *client;

    void updateAddresses();
};

#endif // PEERMGR_HPP

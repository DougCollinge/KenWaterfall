#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>

#include "tcpconnection.hpp"
#include "tcpserver.hpp"

class TCPClient : public QObject
{
    Q_OBJECT

public:
    TCPClient(QString usr, QString addr, quint16 port);
    virtual ~TCPClient();

    int errnum;
    QString errorMessage;

    void sendMessage(const QString &message);
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    QString nickName();

signals:

private slots:

private:
    quint16 m_port;
    QString m_addr;
    QString m_user;

    void removeConnection(TCPConnection *connection);

    TCPServer  *server;
};

#endif // TCPCLIENT_HPP

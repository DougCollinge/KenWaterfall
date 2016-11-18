#ifndef TCPCLIENT_CPP
#define TCPCLIENT_CPP
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QHostInfo>

#include "peermgr.hpp"
#include "tcpconnection.hpp"

#include "tcpclient.hpp"

TCPClient::TCPClient(QString usr, QString addr, quint16 port)
{
    m_port = port;
    m_addr = addr;
    m_user = usr;

    errnum = 0;
    errorMessage = "";
}

TCPClient::~TCPClient()
{
    //
}

void TCPClient::sendMessage(const QString &message)
{
    errnum = 0;
    errorMessage = "";
    if ( message.isEmpty() ) return;
}

QString TCPClient::nickName()
{
    QString me = m_user + "@" + m_addr.trimmed() + QString(":%1").arg(m_port);
    return me;
}

bool TCPClient::hasConnection(const QHostAddress &senderIp, int senderPort) const
{
    return true;
}

#endif // TCPCLIENT_CPP

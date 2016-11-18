#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QSettings>
#include <QtNetwork/QTcpSocket>

#include "tcpserver.hpp"
#include "connectionthread.hpp"

TCPServer::TCPServer(QObject *parent) : QTcpServer(parent)
{
    //
}

void TCPServer::startServer()
{
    quint16 port = 8515;
    QHostAddress addr = QHostAddress("127.0.0.1");

    if( !this->listen(addr, port) )
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    ConnThread *thread = new ConnThread(socketDescriptor, this);
    emit newConnection(socketDescriptor);
    // connect signal/slot
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

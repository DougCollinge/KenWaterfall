#include "connectionthread.hpp"

ConnThread::ConnThread(qintptr ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
}

void ConnThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,

    exec();
}

void ConnThread::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();
    emit newMessage(QString(Data));

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;

    socket->write(Data);
}

void ConnThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";


    socket->deleteLater();
    exit(0);
}


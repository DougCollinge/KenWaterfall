#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

static const int MaxBufferSize = 1024000;

class TCPConnection : public QTcpSocket
{
    Q_OBJECT

public:
    enum ConnectionState
    {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };
    enum DataType
    {
        PlainText,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    TCPConnection(QObject *parent = 0, QString usr = "unknown", QString addr = "127.0.0.1", quint16 port = 8515);

    quint32 errnum;
    QString errorMessage;

    QString name() const;
    void setGreetingMessage(const QString &message);
    bool sendMessage(const QString &message);

signals:
    void readyForUse();
    void newMessage(const QString &from, const QString &message);

protected:
    void timerEvent(QTimerEvent *timerEvent) Q_DECL_OVERRIDE;

private slots:
    void processReadyRead();
    void sendPing();
    void sendGreetingMessage();
    void connectionError(QAbstractSocket::SocketError);

private:
    bool isGreetingMessageSent;

    int numBytesForCurrentDataType;
    int transferTimerId;

    QString greetingMessage;
    QString username;

    QByteArray buffer;

    QTimer pingTimer;
    QTime pongTime;

    ConnectionState state;
    DataType currentDataType;

    void processData();
    bool readProtocolHeader();
    bool hasEnoughData();
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
};

#endif // TCPCONNECTION_HPP

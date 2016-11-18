#include <QtCore/QTimerEvent>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>

#include "tcpconnection.hpp"


static const int TransferTimeout = 30 * 1000; // 30 sec timeout
static const int PongTimeout = 60 * 1000;     // 01 minute timeout
static const int PingInterval = 5 * 1000;     // 05 sec timeout
static const char SeparatorToken = ' ';

TCPConnection::TCPConnection(QObject *parent, QString usr, QString addr, quint16 port) : QTcpSocket(parent)
{
    errnum = 0;
    errorMessage = "";
    greetingMessage = tr("undefined");
    username = usr;
    setLocalAddress(QHostAddress(addr));
    setLocalPort(port);
    state = WaitingForGreeting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    setSocketOption(QAbstractSocket::KeepAliveOption,1);

    pingTimer.setInterval(PingInterval);

    connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage()));
    connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(connectionError(QAbstractSocket::SocketError)));
}

QString TCPConnection::name() const { return username; }

void TCPConnection::setGreetingMessage(const QString &message) { greetingMessage = message; }

bool TCPConnection::sendMessage(const QString &message)
{
    if ( message.isEmpty() )  return false;

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg;
    qint64 er = write(data);
    if ( er < 0 )
    {  // return was -1
        qDebug() << tr("Error writing message:") << errorString();
        return false;
    }
    else if( er == data.size() )
    {
        return true;
    }
    return false;
}

void TCPConnection::timerEvent(QTimerEvent *timerEvent)
{
    if ( timerEvent->timerId() == transferTimerId )
    {
        abort();
        killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

void TCPConnection::connectionError(QAbstractSocket::SocketError err)
{
    QString msg = "";
    switch ( err )
    {
    case AddressInUseError:
        msg = "Address in use";
        break;
    case DatagramTooLargeError:
        msg = "Datagram Too Large";
        break;
    case HostNotFoundError:
        msg = "Host Not Found";
        break;
    case NetworkError:
        msg = "General Network Error";
        break;
    case OperationError:
        msg = "Invalid Network Operation";
        break;
    case ProxyAuthenticationRequiredError:
        msg = "Proxy Authentication Required";
        break;
    case ProxyConnectionClosedError:
        msg = "Proxy Connection Closed";
        break;
    case ProxyConnectionRefusedError:
        msg = "Proxy Connection Refused";
        break;
    case ProxyConnectionTimeoutError:
        msg = "Proxy Connection Timeout";
        break;
    case ProxyNotFoundError:
        msg = "Proxy Not Found";
        break;
    case ProxyProtocolError:
        msg = "Proxy Protocol Violation";
        break;
    case RemoteHostClosedError:
        msg = "Remote Host Closed";
        break;
    case SocketAccessError:
        msg = "Socket Access Error";
        break;
    case SocketAddressNotAvailableError:
        msg = "Socket Address Not Available";
        break;
    case SocketResourceError:
        msg = "Socket Resource Error";
        break;
    case SocketTimeoutError:
        msg = "Socket Timeout";
        break;
    case SslHandshakeFailedError:
        msg = "Ssl Handshake Failure";
        break;
    case SslInternalError:
        msg = "Generalized Ssl Internal Error";
        break;
    case SslInvalidUserDataError:
        msg = "Invalid Ssl User Data";
        break;
    case TemporaryError:
        msg = "Temporary Error";
        break;
    case UnfinishedSocketOperationError:
        msg = "Unfinished Socket Operation";
        break;
    case UnknownSocketError:
        msg = "Unknown Socket Error";
        break;
    case UnsupportedSocketOperationError:
        msg = "Unsupportted Socket Operation";
        break;
    default:
        break;
    }
    if ( msg.length() > 0 ) qDebug() << "TCP/IP Socket Error:" << msg;
}

void TCPConnection::processReadyRead()
{
    if ( state == WaitingForGreeting )
    {
        if ( !readProtocolHeader() )  return;
        if ( currentDataType != Greeting )
        {
            abort();
            return;
        }
        state = ReadingGreeting;
    }

    if ( state == ReadingGreeting )
    {
        if ( !hasEnoughData() )  return;

        buffer = read(numBytesForCurrentDataType);
        if ( buffer.size() != numBytesForCurrentDataType )
        {
            abort();
            return;
        }

        username = QString(buffer) + '@' + peerAddress().toString() + ':' + QString::number(peerPort());
        currentDataType = Undefined;
        numBytesForCurrentDataType = 0;
        buffer.clear();

        if ( !isValid() )
        {
            abort();
            return;
        }

        if ( !isGreetingMessageSent )  sendGreetingMessage();

        pingTimer.start();
        pongTime.start();
        state = ReadyForUse;
        emit readyForUse();
    }

    do
    {
        if ( currentDataType == Undefined )
        {
            if ( !readProtocolHeader() ) return;
        }

        if ( !hasEnoughData() ) return;
        processData();
    } while (bytesAvailable() > 0);
}

void TCPConnection::sendPing()
{
    if ( pongTime.elapsed() > PongTimeout )
    {
        abort();
        return;
    }

    write("PING 1 p");
}

void TCPConnection::sendGreetingMessage()
{
    QByteArray greeting = greetingMessage.toUtf8();
    QByteArray data = "";
    if ( username == "ARDOPMODEM" )
        data = 'C: ' + greeting;
    else
        data = 'c: ' + greeting;
    if ( write(data) == data.size() ) isGreetingMessageSent = true;
}

int TCPConnection::readDataIntoBuffer(int maxSize)
{
    if ( maxSize > MaxBufferSize )  return 0;

    int numBytesBeforeRead = buffer.size();

    if ( numBytesBeforeRead == MaxBufferSize )
    {
        abort();
        return 0;
    }

    while ( (bytesAvailable() > 0) &&
            (buffer.size() < maxSize)
          )
    {
        buffer.append(read(1));
        if ( buffer.endsWith(SeparatorToken) )  break;
    }
    return buffer.size() - numBytesBeforeRead;
}

int TCPConnection::dataLengthForCurrentDataType()
{
    if ( (bytesAvailable() <= 0)     ||
         (readDataIntoBuffer() <= 0) ||
         !buffer.endsWith(SeparatorToken)
       )  return 0;

    buffer.chop(1);
    int number = buffer.toInt();
    buffer.clear();
    return number;
}

bool TCPConnection::readProtocolHeader()
{
    if ( transferTimerId )
    {
        killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if ( readDataIntoBuffer() <= 0 )
    {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    if ( buffer == "PING " )
    {
        currentDataType = Ping;
    }
    else if ( buffer == "PONG " )
    {
        currentDataType = Pong;
    }
    else if ( buffer.left(2) == "c: " )
    {
        currentDataType = PlainText;
    }
    else if ( buffer.left(2) == "C: " )
    {
        currentDataType = PlainText;
    }
    else if ( buffer == "GREETING " )
    {
        currentDataType = Greeting;
    }
    else
    {
        currentDataType = Undefined;
        abort();
        return false;
    }

    buffer.clear();
    numBytesForCurrentDataType = dataLengthForCurrentDataType();
    return true;
}

bool TCPConnection::hasEnoughData()
{
    if ( transferTimerId )
    {
        QObject::killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if ( numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = dataLengthForCurrentDataType();

    if ( (bytesAvailable() < numBytesForCurrentDataType) ||
         (numBytesForCurrentDataType <= 0)
       )
    {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    return true;
}

void TCPConnection::processData()
{
    buffer = read(numBytesForCurrentDataType);
    if ( buffer.size() != numBytesForCurrentDataType )
    {
        abort();
        return;
    }

    switch ( currentDataType )
    {
    case PlainText:
        emit newMessage(username, QString::fromUtf8(buffer));
        break;
    case Ping:
        write("PONG 1 p");
        break;
    case Pong:
        pongTime.restart();
        break;
    default:
        break;
    }

    currentDataType = Undefined;
    numBytesForCurrentDataType = 0;
    buffer.clear();
}


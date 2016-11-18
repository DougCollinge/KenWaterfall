#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>

#include "utils.hpp"
#include "tncpanel.hpp"
#include "tncsetup.hpp"
#include "modem.hpp"


class ARDOPModem::ARDOPModemPrivate
{
public:
    bool blnCMDListener;
    bool blnDataListener;
    bool boolTcpPortOpen;
    bool isGreetingMessageSent;
    bool blnTerminatingConnection;

    quint32 intHostIBCmdPtr;
    quint32 intHostIBDataPtr;

    int bytesToWrite;
    int bytesWritten;
    int bytesReceived;
    int bytesToRcv;
    int bytesAvailable;
    int transferTimerId;
    int TotalBytes;
    int numBytesForCurrentDataType;
    int m_buffers;         // number of buffers set or returned
    int m_bw;              // bandwidth size set or returned

    quint16 blockSize;


    QByteArray buffer;

    QByteArray bytHostIBCmdBuffer;
    QByteArray bytHostIBDataBuffer;
    QByteArray bytLastCMD_DataSent;

    QString id;
    QString myNickName;
    QString username;
    QString greetingMessage;
    QString m_grid;        // gridsquare set or returned
    QString outdata;
};

ARDOPModem::ARDOPModem(QObject *parent) : QObject(parent)
{
#ifdef DEBUG_MODEM
    qDebug() << "Constructor Starting... ";
#endif
    MCB = new ModemControlBlock;
    mp = new ARDOPModemPrivate;
    mp->id = "";

    // Get saved network configuration
#ifdef DEBUG_MODEM
    qDebug() << "reading stored settings... ";
#endif
    readSettings();  // get stored settings for modem use
    m_tcpipaddress = MCB->TCPAddress;
    m_tcpipport    = MCB->TCPPort;
    if ( m_nickname == "" ) m_nickname = "ARDOPMODEM";


    mp->blnCMDListener = false;
    mp->blnDataListener = false;
    mp->boolTcpPortOpen = false;
    mp->isGreetingMessageSent = false;
    mp->numBytesForCurrentDataType = -1;
    mp->transferTimerId = 0;
    mp->intHostIBCmdPtr = 0;
    mp->outdata = "C: ";
    blnClosing = false;
    strLastCommandToHost = "";
    modem_mode = Initializing;  // set modem mode to Idle on startup
    connstate = WaitingForGreeting;
    interface = TCPIP; // set to tcp for testing host interface

#ifdef DEBUG_MODEM
    qDebug() << "Continuing to load ... ";
#endif
    tmrNetworkSend = new QTimer(this);
    tmrNetworkSend->setInterval(30000); // connection send timeout at 30,000 ms or 3 secs.

    // open the TNC GUI Panel
#ifdef DEBUG_MODEM
    qDebug() << "Showing TNC Panel";
#endif
    TNC = new TNCPanel;
    TNC->setParentObject(this);
    TNC->setMCB(MCB);
//    TNC->setRCB(RCB);
    TNC->show();
    clearTNCPanel();

    EnableHostLink();

#ifdef DEBUG_MODEM
    qDebug() << "Loading done";
#endif
}

ARDOPModem::~ARDOPModem()
{
    // try to close and delete host connections here
//    if ( objTCPIPCmdClient ) objTCPIPCmdClient->close();
//    if ( objTCPIPServer ) objTCPIPServer->close();
//    delete objTCPIPCmdClient;
//    delete objTCPIPServer;
    delete TNC;
    delete mp;
}


// ----------------------------------------------------
// EVENT HANDLERS
// ----------------------------------------------------
void ARDOPModem::slotNetworkTimerTimeout()
{
    // interval has ended -- connection has timed out
    tmrNetworkSend->stop();
#ifdef DEBUG_MODEM
    qDebug() << "Server send datagram timeout";
#endif
}

void ARDOPModem::slotSocketStateChg(QAbstractSocket::SocketState s)
{
    // determine the connection state on change and act accordingly
    switch ( s )
    {
    case QAbstractSocket::BoundState:
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::ConnectedState:
        m_hostConnected = true;
        break;
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ListeningState:
        if ( objTCPIPCmdClient->bytesAvailable() > 0 ) processReadyRead();
        break;
    case QAbstractSocket::UnconnectedState:
        m_hostConnected = false;
        break;
    default:
        break;
    }
}

void ARDOPModem::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

#ifdef DEBUG_MODEM
    qDebug() << "...Modem closing";
#endif
    blnClosing = true;
}

// ----------------------------------------------------
// PROPERTY SETS AND GETS
// ----------------------------------------------------
void ARDOPModem::setInterfaceType(const QString typ)
{
    m_interfacetype = typ;                           // set the type description for display and property return
    if (typ == "TCPIP" )     interface = TCPIP;      // set the enum pointer
    if (typ == "SERIAL" )    interface = SERIAL;     //   to be used internally
    if (typ == "BLUETOOTH" ) interface = BLUETOOTH;  //   instead of string compare
}

QString ARDOPModem::getTCPIPAddress()
{
    return MCB->TCPAddress;
}

QString ARDOPModem::getSerialPortName()
{
    return objSerial->objectName();
}

// ----------------------------------------------------
// PUBLIC METHODS AND FUNCTIONS
// ----------------------------------------------------
bool ARDOPModem::SendCommandToHost(QString strText)
{
#ifdef DEBUG_MODEM
    qDebug() << "Sending command to network - " << strText;
#endif
    switch ( interface )
    {
    case TCPIP:
     {
        QString msg = "C:" + strText + "\n";
        objTCPIPCmdClient->write(msg.toLatin1());

        if ( objTCPIPCmdClient->error() < 0 )
        {
#ifdef DEBUG_MODEM
            qDebug() << "Error writing to server:" << objTCPIPCmdClient->errorString();
#endif
            return false;
        }
        else
        {
            return true;
        }
     }

    case SERIAL:
        if ( objSerial && objSerial->isOpen() )
        {
            try
            {
//                objSerial->write(bytToSend, bytToSend.length());
                return true;
            }
            catch (exception ex)
            {
                qCritical() << tr("[HostInterface.SendCommandToHost] Serial Port,  c:") << strText << tr("  error:") << QString().arg(objSerial->error()) << " " << objSerial->errorString();
            }
        }
        else
        {
            qCritical() << tr("[HostInterface.SendCommandToHost] Serial Port ") << MCB->SerCOMPort << tr(" not open");
            return false;
        }
        return false;

    case BLUETOOTH:
        // This will handle BlueTooth connections ...
        // TODO: Add BlueTooth
        return false; // Temporary
    default:
        qCritical() << tr("[HostInterface.SendCommandToHost]  No TCPIP, serial,  or BlueTooth parameters");
    } // switch
    return false;
} // SendCommandToHost()



// ----------------------------------------------------
// PRIVATE SLOTS
// ----------------------------------------------------
void ARDOPModem::slotNewConnection()
{
#ifdef DEBUG_MODEM
    qDebug() << "Setting up TCP Client so modem can listen for datagrams... ";
#endif
    objTCPIPCmdClient = objTCPIPServer->nextPendingConnection();
    connect(objTCPIPCmdClient,SIGNAL(readyRead()),this,SLOT(slotNewMessage()));
    connect(objTCPIPCmdClient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotConnectionError(QAbstractSocket::SocketError)));

    QString msg = "Connected ";
    msg += MCB->TCPAddress;
    msg += ":";
    msg += QString("%1").arg(MCB->TCPPort);
    TNC->ui->lblTCP->setText(msg);
    QString sheet = "QLabel {background-color: #ccffcc; }"; // ColorSystemButton;
    TNC->ui->lblTCP->setStyleSheet(sheet);
    TNC->ui->ledTCP->setEnabled(true);
}

void ARDOPModem::slotNewMessage()
{
#ifdef DEBUG_MODEM
    qDebug() << tr("Getting New Message");
#endif
    QString message = objTCPIPCmdClient->readAll();
#ifdef DEBUG_MODEM
    qDebug() << "Message received:" << message;
#endif
    if ( message.left(2) != "C:" ) ProcessMessage(message);
    if ( message.isEmpty() ) return;
    ProcessMessage(message);
}

void ARDOPModem::removeConnection(QTcpSocket *connection)
{
    if ( peers.contains(connection->peerAddress()) ) peers.remove(connection->peerAddress());
    connection->deleteLater();
}

void ARDOPModem::sessionOpened()
{
}

void ARDOPModem::slotConnected()
{
    mp->boolTcpPortOpen = true;
    if ( MCB->DebugLog )
    {
        qDebug() << tr("Modem connected...")
                 << "\n"
                 << tr("...Modem ")
                 << tr("ready For Use...");
    }
}

void ARDOPModem::slotRemoveConnection(QString nick)
{
    if ( nick.isEmpty() ) return;

    QString msg = "Disconnected ";
    TNC->ui->lblHost->setText(msg);
    QString sheet = "QLabel {background-color: #cccccc; }"; // ColorSystemButton;
    TNC->ui->lblHost->setStyleSheet(sheet);
    TNC->ui->ledConn->setEnabled(false);
}


void ARDOPModem::slotConnectionError(QAbstractSocket::SocketError socketError)
{
    QString msg = "";
    QString msg2 = "";
    switch ( socketError )
    {
    case QAbstractSocket::AddressInUseError:
        msg2 = "Address in use";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        msg2 = "Datagram Too Large";
        break;
    case QAbstractSocket::HostNotFoundError:
        msg2 = "Host Not Found";
        break;
    case QAbstractSocket::NetworkError:
        msg2 = "General Network Error";
        break;
    case QAbstractSocket::OperationError:
        msg2 = "Invalid Network Operation";
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        msg2 = "Proxy Authentication Required";
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        msg2 = "Proxy Connection Closed";
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        msg2 = "Proxy Connection Refused";
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        msg2 = "Proxy Connection Timeout";
        break;
    case QAbstractSocket::ProxyNotFoundError:
        msg2 = "Proxy Not Found";
        break;
    case QAbstractSocket::ProxyProtocolError:
        msg2 = "Proxy Protocol Violation";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        msg2 = "Remote Host Closed";
        break;
    case QAbstractSocket::SocketAccessError:
        msg2 = "Socket Access Error";
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        msg2 = "Socket Address Not Available";
        break;
    case QAbstractSocket::SocketResourceError:
        msg2 = "Socket Resource Error";
        break;
    case QAbstractSocket::SocketTimeoutError:
        msg2 = "Socket Timeout";
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        msg2 = "Ssl Handshake Failure";
        break;
    case QAbstractSocket::SslInternalError:
        msg2 = "Generalized Ssl Internal Error";
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        msg2 = "Invalid Ssl User Data";
        break;
    case QAbstractSocket::TemporaryError:
        msg2 = "Temporary Error";
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        msg2 = "Unfinished Socket Operation";
        break;
    case QAbstractSocket::UnknownSocketError:
        msg2 = "Unknown Socket Error";
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        msg2 = "Unsupported Socket Operation";
        break;
    default:
        break;
    }

    msg  = tr("Modem ") + objTCPIPServer->serverAddress().toString();
    msg += ":";
    msg += QString("%1").arg(objTCPIPServer->serverPort());
    qCritical() << msg;
    msg = tr("Connection Error . . .");
    msg += QString("%1").arg(socketError);
    qCritical() << msg;

    if ( QTcpSocket *connection = qobject_cast<QTcpSocket *>(sender()) ) removeConnection(connection);
}

void ARDOPModem::slotUpdateClient(qint64 b)
{
    Q_UNUSED(b)
//    bytesReceived += (int)b;
//    buffer = tcpconnection->readAll();
    if ( MCB->DebugLog ) qDebug() << "Client Rcvd msg: " << mp->buffer;
//    if ( bytesReceived == TotalBytes || tcpconnection->atEnd() )
//    {
        //processBytes();
        //tcpconnection->close();
//    }
}


// ----------------------------------------------------
// PRIVATE METHODS AND FUNCTIONS
// ----------------------------------------------------
void ARDOPModem::ProcessMessage(QString indata)
{
#ifdef DEBUG_MODEM
    qDebug() << "...Modem received message....";
#endif
    QString pdata = indata.trimmed(); // gets rid of the \n
    QStringList cmd = pdata.split(" ",QString::KeepEmptyParts);

#ifdef DEBUG_MODEM
    qDebug() << "Rcvd message size = " << QString("%1").arg(indata.size()-1);
#endif
    // the "C:" says it is a server side command - so
    // the command parser should ignore it
    if ( cmd.at(0) == "C:") return;
    // only contains "c:"
    if ( cmd.count() == 1 && cmd.at(0) == "c:") return;

    if ( !pdata.endsWith("\0" ) )
    {
#ifdef DEBUG_MODEM
        qDebug() << "Rcvd message ends w/NULL -  " << indata;
#endif
    }
    else if ( !pdata.endsWith("\n" ) )
    {
        if ( MCB->DebugLog ) qDebug() << "Rcvd message ends w/CR -  " << indata;
    }
    if ( MCB->DebugLog ) qDebug() << "Checking Rcvd message type";

    qDebug() << "PDATA:" << pdata;
    for ( int x = 0; x < cmd.count(); ++x)
    {
#ifdef DEBUG_MODEM
        qDebug() << "CMD: "  << cmd.at(x);
#endif
    }

    if ( cmd.at(1) == "ABORT" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "ABORT command executed....exiting";
#endif
        TNC->close();
        delete TNC;
        exit(0);
    }
    else if ( cmd.at(1) == "CMD" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "Command sent " << pdata;
#endif
    }
    else if ( cmd.at(1) == "VERSION" )
    {
        QString msgstr = "Echoing Version at Server: ";
        QString msg = APP_VERSION;
        qDebug() << msgstr << msg;

        QByteArray data = QString("C: VERSION ").toUtf8();
        data += APP_VERSION;
        if ( !objTCPIPCmdClient->isOpen() ) objTCPIPCmdClient->open(QTcpSocket::WriteOnly);
        objTCPIPCmdClient->write(data,data.length());
        objTCPIPCmdClient->close();
        return;
    }
    else if ( cmd.at(1) == "MODE" )
    {
        quint16 modevector = AllModes.indexOf(cmd.at(2),0);
        switch (modevector)
        {
        case 0:
            MCB->ProtocolMode = "8FSK.200.25";
            qDebug() << "Changing mode to " << MCB->ProtocolMode;
            break;
        case 1:
            MCB->ProtocolMode = "4FSK.200.50S";
            break;
        case 2:
            MCB->ProtocolMode = "4FSK.200.50";
            break;
        case 3:
            MCB->ProtocolMode = "4PSK.200.100S";
            break;
        case 4:
            MCB->ProtocolMode = "4PSK.200.100";
            break;
        case 5:
            MCB->ProtocolMode = "8PSK.200.100";
            break;
        case 6:
            MCB->ProtocolMode = "16FSK_500.25S";
            break;
        case 7:
            MCB->ProtocolMode = "16FSK.500.25";
            break;
        case 8:
            MCB->ProtocolMode = "4FSK.500.100S";
            break;
        case 9:
            MCB->ProtocolMode = "4FSK.500.100";
            break;
        case 10:
            MCB->ProtocolMode = "4PSK.500.100";
            break;
        case 11:
            MCB->ProtocolMode = "8PSK.500.100";
            break;
        case 12:
            MCB->ProtocolMode = "4PSK.500.167";
            break;
        case 13:
            MCB->ProtocolMode = "8PSK.500.167";
            break;
        case 14:
            MCB->ProtocolMode = "4FSK.1000.100";
            break;
        case 15:
            MCB->ProtocolMode = "4PSK.1000.100";
            break;
        case 16:
            MCB->ProtocolMode = "8PSK.1000.100";
            break;
        case 17:
            MCB->ProtocolMode = "4PSK.1000.167";
            break;
        case 18:
            MCB->ProtocolMode = "8PSK.1000.167";
            break;
        case 19:
            MCB->ProtocolMode = "4FSK.2000.600S";
            break;
        case 20:
            MCB->ProtocolMode = "4FSK.2000.600";
            break;
        case 21:
            MCB->ProtocolMode = "4FSK.2000.100";
            break;
        case 22:
            MCB->ProtocolMode = "4PSK.2000.100";
            break;
        case 23:
            MCB->ProtocolMode = "8PSK.2000.100";
            break;
        case 24:
            MCB->ProtocolMode = "4PSK.2000.167";
            break;
        case 25:
            MCB->ProtocolMode = "8PSK.2000.167";
            break;
        default:
            MCB->ProtocolMode = "4PSK.500.100";
            break;
        }
        QString msg = "Rx Frame ";
        msg += MCB->ProtocolMode;
        TNC->ui->lblRxFrameTxt->setText(msg);
        QString sheet = "QLabel {background-color: #ccffcc; }"; // ColorSystemButton;
        TNC->ui->lblRxFrameTxt->setStyleSheet(sheet);
    }
    else
    {
        QStringList l = indata.split(' ',QString::KeepEmptyParts);
        QString strlist;
        if ( MCB->DebugLog )  qDebug() << "Strings " << strlist;
    }
    mp->buffer.chop(1);
    processCMD(QString(mp->buffer.data()));
    mp->buffer.clear();
    if ( MCB->DebugLog ) qDebug() << "Received command verified: " << mp->buffer;
}

bool ARDOPModem::TerminateHostLink()
{
    mp->blnTerminatingConnection = true;
    Status stcStatus;

    switch ( interface )
    {
    case TCPIP:
        try
        {
            QString msg = "Disconnecting...";
            TNC->ui->lblTCP->setText(msg);
            QString sheet = "QLabel {background-color: #ff9933; }"; // ColorKacki;
            TNC->ui->lblTCP->setStyleSheet(sheet);
            TNC->ui->ledTCP->setEnabled(false);
            /*
            if ( objTCPData )
            {
                objTCPIPDataClient->close();
                delete objTCPIPDataClient;
                delete objTCPData;
            }
            strTCPIPControlConnectionID = "";
            strTCPIPDataConnectionID = "";
            */

            // We leave listening socket, so
            // can accept another connection

        }
        catch (exception Ex)
        {
            qCritical() << tr("[HostInterface.TerminateHostLink]  error: ") << Ex.what();
            stcStatus.BackColor = ColorLightSalmon; // set to red ....
            emit signalCtrlChgd(stcStatus.ControlName,stcStatus.BackColor,stcStatus.Text,stcStatus.Value);
            MCB->LinkedToHost = false;
            return false;
        }
        MCB->LinkedToHost = false;
        return true;

    case SERIAL:
        break;
    case BLUETOOTH:
        break;
    default:
        qCritical() << tr("[HostInterface.TerminateHostLink]  Interface Type not set");
    }
    return false;
}  // TerminateHostLink()

void ARDOPModem::processReadyRead()
{
    if ( MCB->DebugLog )  qDebug() << "Processing ReadyRead Data";

 //   readProtocolHeader(); // determine header type
    if ( connstate == WaitingForGreeting )
    {
        if ( MCB->DebugLog )
            qDebug() << "Processing ReadyRead Reading Data - waiting for greeting";

//        readProtocolHeader(); // determine header type
        //buffer = tcpconnection->read(numBytesForCurrentDataType);
        if ( MCB->DebugLog )
        {
            QString msgstr;
            msgstr = "Processing ReadyRead Reading Data - data rcvd" + mp->buffer + " size: " + QString("%1").arg(mp->buffer.size());
            qDebug() << msgstr;
        }

        if ( mp->buffer.size() == 0 )
        {
#ifdef DEBUG_MODEM
            qDebug() << "Modem...not enough buffer data returning";
            qDebug() << "Modem...buffer data" << mp->buffer.size();
            qDebug() << "Modem...number of bytes " << mp->buffer.size();
#endif
            return;
        }
#ifdef DEBUG_MODEM
        qDebug() << "Modem reading greeting";
#endif
//        if ( currentDataType != Greeting)   return;

#ifdef DEBUG_MODEM
        qDebug() << "Data Type Undefined";
#endif
//        QString username = QString(buffer) + '@' + tcpconnection->peerAddress().toString() + ':' + QString::number(tcpconnection->peerPort());
        mp->buffer.clear();

#ifdef DEBUG_MODEM
        qDebug() << "New state ReadingGreeting";
#endif
        if ( !mp->isGreetingMessageSent )
        {
            mp->greetingMessage = "greeting";
            //sendGreetingMessage();
        }
        connstate = ReadyForUse;
        return;
    }

#ifdef DEBUG_MODEM
    qDebug() << "process data";
    qDebug() << "Greeting found - New state...ReadyForUse";
#endif
    connstate = ReadyForUse;
}


void ARDOPModem::processData()
{
/*
    if ( MCB->DebugLog ) qDebug() << "Reading data to buffer";
    buffer = tcpconnection->read(numBytesForCurrentDataType);
    if ( MCB->DebugLog ) qDebug() << "If bytes read != num bytes for data type";
    if (buffer.size() != numBytesForCurrentDataType)   return;
    if ( MCB->DebugLog ) qDebug() << "Checking data type";
    switch (currentDataType)
    {
    case Greeting:
        if ( MCB->DebugLog )
            qDebug() << "New greeting arived: " << QString::fromUtf8(buffer);
        return;
        break;
    case PlainText:
        if ( MCB->DebugLog )
        {
            qDebug() << "New message arived: " << QString::fromUtf8(buffer);
            qDebug() << "Sending Command to Interpreter ";
        }
        commandInterpreter(buffer);
        break;
    case ModeExact:
        if ( MCB->DebugLog )
        {
            qDebug() << "New message arived: " << QString::fromUtf8(buffer);
            qDebug() << "Sending Command to Interpreter ";
        }
        commandInterpreter(buffer);
        break;
    case Ping:
        tcpconnection->write("PONG 1 p");
        break;
    case Pong:
        pongTime.restart();
        break;
    default:
        currentDataType = Undefined;
        numBytesForCurrentDataType = 0;
        buffer.clear();
        if ( MCB->DebugLog )
        {
            qDebug() << "Data Type Undefined";
            qDebug() << "...Clearing buffer ... done processingData";
        }
        break;
    }
*/
}

bool ARDOPModem::readCMDHeader()
{
#ifdef DEBUG_MODEM
    qDebug() << "Reading Protocol Header";
#endif
    mp->buffer.clear();
#ifdef DEBUG_MODEM
    qDebug() << "ReadProtocolHeader reading data into buffer";
#endif
//    readDataIntoBuffer(MaxBufferSize);
    QList<QByteArray> incmd;
    incmd << mp->buffer.split(' ');
    QString msgstr;
    msgstr = "parsing data from buffer - " + mp->buffer;
    msgstr = "  mode: " + incmd[0];
    if ( MCB->DebugLog ) qDebug() << msgstr;
    if ( incmd[0] == "CLOSE" )
    {   // if command is to close do it now.
        exit(0);
    }

#ifdef DEBUG_MODEM
    qDebug() << "Reading Protocol Header - returning data: " << mp->buffer;
#endif
    return true;
}

bool ARDOPModem::hasEnoughData()
{
#ifdef DEBUG_MODEM
    qDebug() << "checking for data - correct number of bytes: " << mp->numBytesForCurrentDataType;
#endif
    if ( mp->buffer.size() <= 0 )
    {
#ifdef DEBUG_MODEM
        qDebug() << "bytes available < data type or:\n"
                 << "num bytes not enough - starting timer \n"
                 << "buffer bytes: "
                 << mp->buffer.size() << "\n"
                 << "tcpconnection bytes= "
//                 << QString("%1").arg(tcpconnection->bytesAvailable())
                 << "bytes for data type: "
                 << QString("%1").arg(mp->numBytesForCurrentDataType)
                 << "\n checked data returning false";
#endif
        return false;
    }
#ifdef DEBUG_MODEM
    qDebug() << "checked data good - returning true ";
#endif
    return true;
}

int ARDOPModem::dataLengthForCurrentDataType()
{
/*
    if ( MCB->DebugLog ) qDebug() << "checking for data length";
    if ( buffer.size() <= 0 )  return 0;
    if ( MCB->DebugLog ) qDebug() << "bytesAvailable > 0";
    if ( readDataIntoBuffer(MAX_BUF_SIZE) <= 0 )  return 0;
    if ( MCB->DebugLog ) qDebug() << "good data in buffer";
    int number = buffer.size();
    if ( MCB->DebugLog ) qDebug() << "will return if not ends w/token";
    if ( !buffer.endsWith(SeparatorToken) ) return number;
    if ( MCB->DebugLog ) qDebug() << "ends with separator token";
    buffer.chop(1);
    number = buffer.size();
    if ( MCB->DebugLog ) qDebug() << "received data length " << number;
    return number;
*/
    return 0;
}


void ARDOPModem::sendMessage(const char *message)
{
#ifdef DEBUG_MODEM
    qDebug() << "got " << QString(message).toUtf8();
#endif
    QByteArray msg = QString(message).toUtf8();
    QByteArray data = "MESSAGE " + ' ' + msg;
#ifdef DEBUG_MODEM
    qDebug() << "Modem sending message " << data;
#endif
    try
    {
        if ( !mp->boolTcpPortOpen )
        {
#ifdef DEBUG_MODEM
            qDebug() << "Modem connection not open for write";
#endif
//            tcpconnection->connectToHost(QHostAddress("127.0.0.1"),8600);
            mp->boolTcpPortOpen  = true;
        }
#ifdef DEBUG_MODEM
        qDebug() << "Modem writing to host";
#endif
//        tcpconnection->write(data); //== data.size();
//        tcpconnection->disconnectFromHost();
    }
    catch (...)
    {
//        if ( MCB->DebugLog ) qCritical() << "Modem Socket Error connecting for write:" << tcpconnection->errorString();
    }

    try
    {
//        tcpconnection->connectToHost(id, 8600);
    }
    catch (...)
    {
//        if ( MCB->DebugLog ) qCritical() << "Modem Socket Error opening port for write:" << tcpconnection->errorString();
    }
//    boolTcpPortOpen = tcpconnection->isOpen();
    mp->boolTcpPortOpen = false;
#ifdef DEBUG_MODEM
    qDebug() << "Modem port to host open for write: " << mp->boolTcpPortOpen;
#endif
//    tcpconnection->write(message,sizeof(message));
}

int ARDOPModem::readDataIntoBuffer(int x)
{
    Q_UNUSED(x);

#ifdef DEBUG_MODEM
    qDebug() << "Starting...";
#endif
//    if ( buffer.size() > MaxBufferSize ) return -1;
#ifdef DEBUG_MODEM
    qDebug() << "Data less than max";
#endif
    int numBytesBeforeRead = mp->buffer.size();
//    if ( numBytesBeforeRead == MaxBufferSize ) return 0;
#ifdef DEBUG_MODEM
    qDebug() << "Max not reached";
#endif
/*
    while ( tcpconnection->bytesAvailable() > 0 && buffer.size() < MAX_BUF_SIZE )
    {
        if ( MCB->DebugLog ) qDebug() << "Appending data";
        buffer.append(tcpconnection->read(1));
        //if (buffer.endsWith(SeparatorToken))   break;
    }
#ifdef DEBUG_MODEM
         QString msg;
         msg = "Data appended returning Size: ";
         msg += QString("%1").arg(buffer.size());
         msg += " B4: ";
         msg += QString("%1").arg(numBytesBeforeRead);
         qDebug() << msg;
#endif
*/
    int z = mp->buffer.size() - numBytesBeforeRead;
    if (z == 0) z = mp->buffer.size();
#ifdef DEBUG_MODEM
    qDebug() << "returning ...";
#endif
    return z;
}

void ARDOPModem::disconnect()
{
//    if ( tcpconnection->isOpen() ) tcpconnection->disconnect();
    mp->boolTcpPortOpen = false;
#ifdef DEBUG_MODEM
    qDebug() << "closing connection";
#endif
}

void ARDOPModem::disconnected()
{
#ifdef DEBUG_MODEM
    qDebug() << "connection disconnected and closing";
#endif

//    tcpconnection->close();
#ifdef DEBUG_MODEM
    qDebug() << "connection closed";
#endif
}

void ARDOPModem::connectionError()
{
//    errNo = tcpconnection->error();
//    errMsg  = tcpconnection->errorString();
#ifdef DEBUG_MODEM
        qDebug() << "Unexpected Connection Error: "
                 << QString().arg(errNo)
                 << " - " << errMsg;
#endif
}

bool ARDOPModem::hasConnection(const QHostAddress &senderIp, int senderPort) const
{
    if ( senderPort == -1 ) return peers.contains(senderIp);

    if ( !peers.contains(senderIp) ) return false;

    QList<QTcpSocket *> connections = peers.values(senderIp);
    foreach (QTcpSocket *connection, connections)
    {
        if ( connection->peerPort() == senderPort )  return true;
    }
    QString msg = "Connected ";
    msg += MCB->TCPAddress;
    msg += ":";
    msg += QString("%1").arg(MCB->TCPPort);
    TNC->ui->lblTCP->setText(msg);
    QString sheet = "QLabel {background-color: #ccffcc; }"; // ColorLightGreen;
    TNC->ui->lblTCP->setStyleSheet(sheet);
    TNC->ui->ledTCP->setEnabled(true);
    return false;
}


// =========================================================
// PUBLIC METHODS
// =========================================================

// =========================================================
// PRIVATE FUNCTIONS
// =========================================================
bool ARDOPModem::EnableHostLink()
{
#ifdef DEBUG_MODEM
    qDebug() << "Enabling Host Link";
#endif
    mp->blnTerminatingConnection = false;
    Status stcStatus;
//    qint16 Trace = 0;

    MCB->LinkedToHost = false;
    switch ( interface )
    {
    case TCPIP:
#ifdef DEBUG_MODEM
        qDebug() << "Interface Type is TCP/IP";
#endif
        stcStatus.Text = "TCPIP";
        try
        {
            objTCPIPServer = new QTcpServer(this);
            if ( !objTCPIPServer->listen(QHostAddress(m_tcpipaddress),m_tcpipport) )
            {
                qCritical() << tr("Unable to start the server: ") << objTCPIPServer->errorString();
                return false;
            }
            if ( !objTCPIPServer->isListening() )
            {
                qCritical() << tr("Server is not listening: ") << objTCPIPServer->errorString();
                return false;
            }
            // use IPv4 localhost
            if ( m_tcpipaddress.isEmpty() ) m_tcpipaddress = objTCPIPServer->serverAddress().toString();
#ifdef DEBUG_MODEM
            qDebug() << tr("The server is running on") << QString("IP: %1 port: %2").arg(m_tcpipaddress).arg(objTCPIPServer->serverPort());
#endif
            connect(objTCPIPServer,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));

            // Start listening for client requests.
            //mp->blnCMDListener = objTCPIPCmdListener->listen(QHostAddress("127.0.0.1"),8515);
//            if ( !mp->blnCMDListener )
//            {
//                qCritical() << tr("TCP/IP Server Unable to start the server: ") << objTCPIPCmdListener->errorString();
//                close();
//                return false;
//            }
//            qDebug() << "Connecting Listener to local slots";
//            connect(objTCPIPCmdListener,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
/*
            if ( !objTCPIPCmdListener->isListening() )
            {
#ifdef DEBUG_MODEM
                qDebug() << tr("The server had an error trying to listen on")
                         << tr("IP: %1").arg(MCB->TCPAddress)
                         << tr(":%1").arg(MCB->TCPPort)
                         << tr("\nError:") << objTCPIPCmdListener->errorString();
#endif
                close();
                return false;
            }
            else
            {
#ifdef DEBUG_MODEM
                qDebug() << tr("The server is running on")
                         << tr("IP: %1").arg(MCB->TCPAddress)
                         << tr(":%1").arg(MCB->TCPPort);
#endif
            }
*/
//            qDebug() << "Setting up Data listener";
//            objTCPIPDataListener = new QTcpServer(this);
//            mp->blnDataListener = objTCPIPDataListener->listen(QHostAddress(MCB->TCPAddress), MCB->TCPPort + 1);
#ifdef DEBUG_MODEM
            qDebug() << "Sending signal to TNC Panel for display change";
#endif
            QString msg = MCB->TCPAddress;
            msg += ":";
            msg += QString("%1").arg(MCB->TCPPort);
            TNC->ui->lblTCP->setText(msg);
            QString sheet = "QLabel {background-color: #ddffdd; }"; // ColorSystemButton;
            TNC->ui->lblTCP->setStyleSheet(sheet);
            TNC->ui->ledTCP->setEnabled(true);
#ifdef DEBUG_MODEM
            qDebug() << tr("objTCPIPDataLister Listening  on ") << MCB->TCPAddress << tr(" Port:") << QString("%1").arg(MCB->TCPPort);
#endif
            return true;
        }
        catch (exception Ex)
        {
            QString msg = "Error...";
            TNC->ui->lblTCP->setText(msg);
            QString sheet = "QLabel {background-color: #ffa366; }"; // ColorSystemButton;
            TNC->ui->lblTCP->setStyleSheet(sheet);
            TNC->ui->ledTCP->setEnabled(false);
            QApplication::processEvents();
            mp->blnCMDListener = false;
            mp->blnDataListener = false;
            qCritical() << "[HostInterface.EnableHostLink]  TCPIP link startup, Err: " << Ex.what();
            return false;
        }
        mp->bytHostIBCmdBuffer.clear();
        mp->bytHostIBDataBuffer.clear();

        mp->intHostIBDataPtr = 0;
        mp->intHostIBCmdPtr = 0;
#ifdef DEBUG_MODEM
        qDebug() << "Host TCP/IP Interface ready...";
#endif
        return true;

    case SERIAL:
        stcStatus.Text = "Host Serial";
        if ( !objSerial ) objSerial = new QSerialPort(this);

        objSerial->setPortName(MCB->SerCOMPort);
        objSerial->setBaudRate(MCB->SerBaud);
        objSerial->setParity(QSerialPort::NoParity);
        objSerial->setStopBits(QSerialPort::OneStop);

        try
        {
            objSerial->open(QSerialPort::ReadWrite);
            objSerial->readAll();
        }
        catch (exception Ex)
        {
            qCritical() << tr("[HostInterface.EnableHostLink]  Serial link startup, Error: ") << Ex.what();
            stcStatus.BackColor = ColorLightSalmon; // set to red ....
            emit signalCtrlChgd(stcStatus.ControlName,stcStatus.BackColor,stcStatus.Text,stcStatus.Value);
            return false;
        }
        break;

    case BLUETOOTH:
        stcStatus.Text = "Host BlueTooth";
        // BlueTooth link initialization here????
        return true;
    default:
        break;
    }
    return false;
}

void ARDOPModem::processCMD(QString cmd)
{
    Q_UNUSED(cmd)
#ifdef DEBUG_MODEM
    qDebug() << "Processing CMD: " << cmd;
#endif
}

QString ARDOPModem::nickName() const
{
    QString strMsg;
    strMsg = MCB->TCPAddress;
    strMsg += '@';
    strMsg += "ARDOPHOST";
    strMsg += ':';
    strMsg += QString("%1").arg(MCB->TCPPort);
    return strMsg;
}



void ARDOPModem::commandInterpreter(QString cmd)
{
    // start up the appropriate TNC or pass settings to codec
    QList<QString> incmd = cmd.split(" ",QString::KeepEmptyParts);
#ifdef DEBUG_MODEM
    QString msgstr = "Interpreting CMD - " + incmd[0] + " " + incmd[1];
    if ( MCB->DebugLog ) qDebug() << msgstr;
#endif
    if ( incmd[0] == "MODE" )
    {
//        quint16 modevector = AllModes.contains(incmd.at(1));
        quint16 modevector = 0;
        switch ( modevector )
        {
        // decypher mode
        case 0:
//            MCB->ProtocolMode = AllModes.at(0);
        default:
            MCB->ProtocolMode = "4PSK.500.100";
        }
    }
    // not mode so decypher which command
    else if ( incmd[0] == "CLOSE" )
    {
#ifdef DEBUG_MODEM
        qDebug() <<"CLOSE cmd";
#endif
        disconnect();
        mp->buffer = 0;
        exit(0);
    }
    else if ( incmd[0] == "AUTOBREAK" )
    {
#ifdef DEBUG_MODEM
        qDebug() <<"AutoBreak rcvd";
#endif
        if ( incmd.size() == 1 )
        {
            sendMessage("AUTOBREAK");
            return;
        }
//        if ( incmd.size() > 1 ) MCB->AutoBREAK = false;    // if value is changing reset
//        if ( incmd[1] == "true" ) MCB->AutoBREAK = true;  // set value to value sent
        // decypher mode
        /*
        switch (m_mode)
        {
        case ModeIdle:
            return;
            break;
        case WinMor500:
            //winmor_codec->autoBreak(OMCB.AutoBREAK);
            break;
        case WinMor1500:
            //winmor_codec->autoBreak(OMCB.AutoBREAK);
            break;
        case Pactor500:
            pactor_codec->autoBreak(OMCB.AutoBREAK);
            break;
        case Pactor1500:
            pactor_codec->autoBreak(OMCB.AutoBREAK);
            break;
        default:
            break;
        }
        */
    }
    else if ( incmd[0] == "BREAK" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "BREAK rcvd";
#endif
        // decypher mode
        /*
        switch (m_mode)
        {
        case ModeIdle:
            return;
            break;
        case WinMor500:
            //winmor_codec->sendBreak();
            break;
        case WinMor1500:
            //winmor_codec->sendBreak();
            break;
        case Pactor500:
            pactor_codec->sendBreak();
            break;
        case Pactor1500:
            pactor_codec->sendBreak();
            break;
        default:
            break;
        }
        */
    }
    else if ( incmd[0] == "BUFFERS" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "BUFFERS rcvd";
#endif
        // decypher mode
        /*
        switch (m_mode)
        {
        case ModeIdle:
            return;
            break;
        case WinMor500:
            //m_buffers = winmor_codec->buffers();
            break;
        case WinMor1500:
            //m_buffers = winmor_codec->buffers();
            break;
        case Pactor500:
            //we have to define a local for formating a BUFFERS <num> message to send
            //m_buffers = pactor_codec->buffers();
            // format new message here and send on TCP/IP connection
            break;
        case Pactor1500:
            //we have to define a local for formating a BUFFERS <num> message to send
            //m_buffers = pactor_codec->buffers();
            // format new message here and send on TCP/IP connection
            break;
        default:
            break;
        }
        */
    }
    else if ( incmd[0] == "BUSYLOCK" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "BUSYLOCK rcvd";
#endif
    }
    else if ( incmd[0] == "BW" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "BW rcvd";
#endif
        // BW command  params 500 / 1500 / none
        //   will get set bandwith if no parms
        //
        // decypher mode
        /*
        switch (m_mode)
        {
        case ModeIdle:
            return;
            break;
        case WinMor500:
            if ( incmd.size() == 2 )
            {
                int intBW;
                intBW = incmd[1].toInt();
                OMCB.Bandwidth = intBW;
                //we have to define a local for formating a BUFFERS <num> message to send
#ifdef DEBUG_MODEM
                 qDebug()<< "BW cmd rcvd and setting BW in codec";
#endif
                m_bw = false; //winmor_codec->setBW(intBW);
               // format new message here and send on TCP/IP connection
            }
            else if ( incmd.size() == 1)
            {
                // format new message here and send on TCP/IP connection
                sendMessage("BW " + OMCB.Bandwidth);
            }
            break;
        case WinMor1500:
            if ( incmd.size() == 2 )
            {
                int intBW;
                intBW = incmd[1].toInt();
                OMCB.Bandwidth = intBW;
                //we have to define a local for formating a BUFFERS <num> message to send
#ifdef DEBUG_MODEM
                qDebug() << "BW cmd rcvd and setting BW in codec";
#endif
                m_bw = false; //winmor_codec->setBW(intBW);
               // format new message here and send on TCP/IP connection
            }
            else if ( incmd.size() == 1)
            {
                // format new message here and send on TCP/IP connection
#ifdef DEBUG_MODEM
                 qDebug() << "BW cmd rcvd returning from codec";
#endif
                sendMessage("BW " + OMCB.Bandwidth);
            }
            break;
        case Pactor500:
            if ( incmd.size() == 2 )
            {
                int intBW;
                intBW = incmd[1].toInt();
                //we have to define a local for formating a BUFFERS <num> message to send
                m_bw = pactor_codec->setBW(intBW);
               // format new message here and send on TCP/IP connection
            }
            else if ( incmd.size() == 1)
            {
                //we have to define a local for formating a BUFFERS <num> message to send
                m_bw = pactor_codec->setBW(NULL);
               // format new message here and send on TCP/IP connection
            }
            break;
        case Pactor1500:
            if ( incmd.size() == 2 )
            {
                int intBW;
                intBW = incmd[1].toInt();
                //we have to define a local for formating a BUFFERS <num> message to send
                m_bw = pactor_codec->setBW(intBW);
               // format new message here and send on TCP/IP connection
            }
            else if ( incmd.size() == 1)
            {
                //we have to define a local for formating a BUFFERS <num> message to send
                m_bw = pactor_codec->setBW(NULL);
               // format new message here and send on TCP/IP connection
            }
            break;

        default:
            break;
        }
        */
    }
    else if ( incmd[0] == "CAPTURE" )
    {
        if ( MCB->DebugLog ) qDebug() << "CAPTURE cmd";
    }
    else if ( incmd[0] == "CAPTUREDEVICES" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "CAPTUREDEVICES cmd";
#endif
    }
    else if ( incmd[0] == "CODEC" )
    {
#ifdef DEBUG_MODEM
        qDebug() << "CODEC cmd";
#endif
        if ( incmd.size() == 1 )
        {
#ifdef DEBUG_MODEM
            qDebug() << "CAPTURE cmd W/NO PARMS";
#endif
            // return codec in formatted string  //
            /*
            switch ( OMCB.activeCodec )
            {
            case WinMor500:
                sendMessage("CODEC WINMOR500");
                break;
            case WinMor1500:
                sendMessage("CODEC WINMOR1500");
                break;
            case Pactor500:
                sendMessage("CODEC PACTOR500");
                break;
            case Pactor1500:
                sendMessage("CODEC PACTOR1500");
                break;
            default:
                break;
            }
            */
            return;
        } // end return current codec
        // process parm 2  //
    }
    else if ( incmd[0] == "CONNECT" )
    {
    }
    else if ( incmd[0] == "CONNECTED" )
    {
    }
    else if ( incmd[0] == "DEBUGLOG" )
    {
    }
    else if ( incmd[0] == "DIRTYDISCONNECT" )
    {
    }
    else if ( incmd[0] == "DISCONNECT" )
    {
    }
    else if ( incmd[0] == "DRIVELEVEL" )
    {
        if ( incmd.size() == 1 )
        {
            sendMessage("DRIVELEVEL " + MCB->TxDriveLevel);
            return;
        }
        else
        {
            quint32 m_driveLvl;
            m_driveLvl = incmd[1].toInt();
            MCB->TxDriveLevel = m_driveLvl;
            /*
            switch (m_mode)
            {
            case ModeIdle:
                break;
            case WinMor500:
                //winmor_codec->driveLevel(m_driveLvl);
                break;
            case WinMor1500:
                //winmor_codec->driveLevel(m_driveLvl);
                break;
            case Pactor500:
                //pactor_codec->driveLevel(m_driveLvl);
                break;
            case Pactor1500:
                //pactor_codec->driveLevel(m_driveLvl);
                break;
            case Packet300:
                break;
            case Packet1200:
                break;
            }
            */
        }
    }
    else if ( incmd[0] == "FAULT" )
    {
    }
    else if ( incmd[0] == "FECRCV" )
    {
    }
    else if ( incmd[0] == "FECSEND" )
    {
    }
    else if ( incmd[0] == "GRIDSQUARE" )
    {
        if ( incmd.size() == 1)
        {
            // GRIDSQUARE message not returned here
            sendMessage(QString("GRIDSQUARE " + MCB->GridSquare).toLatin1());
        }
        else
        {
            QString m_grid = incmd[1].toUtf8();
            MCB->GridSquare = m_grid;
            /*
            switch (m_mode)
            {
            case Pactor500:
                //pactor_codec->gridSquare(m_grid);
                break;
            case Pactor1500:
                //pactor_codec->gridSquare(m_grid);

            default:
                break;
            }
            */
        }
    }
    else if ( incmd[0] == "ISREGISTERED" )
    {
    }
    else if ( incmd[0] == "LEADEREXT" )
    {
    }
    else if ( incmd[0] == "LISTEN" )
    {
    }
    else if ( incmd[0] == "MAXCONREQ" )
    {
    }
    else if ( incmd[0] == "MONCALL" )
    {
    }
    else if ( incmd[0] == "MYAUX" )
    {
    }
    else if ( incmd[0] == "MYC" )
    {
    }
    else if ( incmd[0] == "NEWSTATE" )
    {
    }
    else if ( incmd[0] == "OFFSET" )
    {
    }
    else if ( incmd[0] == "OUTQUEUED" )
    {
    }
    else if ( incmd[0] == "OVER" )
    {
    }
    else if ( incmd[0] == "PLAYBACK" )
    {
    }
    else if ( incmd[0] == "PLAYBACKDEVICES" )
    {
    }
    else if ( incmd[0] == "PORTS" )
    {
    }
    else if ( incmd[0] == "PROCESSID" )
    {
    }
    else if ( incmd[0] == "RESPONSEDLY" )
    {
    }
    else if ( incmd[0] == "SENDID" )
    {
    }
    else if ( incmd[0] == "SHOW" )
    {
    }
    else if ( incmd[0] == "SQUELCH" )
    {
    }
    else if ( incmd[0] == "STATE" )
    {
    }
    else if ( incmd[0] == "SUFFIX" )
    {
    }
    else if ( incmd[0] == "TARGET" )
    {
    }
    else if ( incmd[0] == "TWOTONETEST" )
    {
    }
    else if ( incmd[0] == "VERSION" )
    {
    }
    else if ( incmd[0] == "VOX" )
    {
    }
}

void ARDOPModem::appendMessage(const QString &from, const QString &message)
{
    if ( from.isEmpty() || message.isEmpty() )   return;
}

void ARDOPModem::showInformation()
{
    QString title = tr("ARDOP Modem");
    qInfo() << title;
}

void ARDOPModem::updateAddresses()
{
    broadcastAddresses.clear();
    ipAddresses.clear();
    foreach ( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
    {
        foreach ( QNetworkAddressEntry entry, interface.addressEntries() )
        {
            QHostAddress broadcastAddress = entry.broadcast();
            if ( broadcastAddress != QHostAddress::Null &&
                 (entry.ip() != QHostAddress::LocalHost)
               )
            {
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }
    }
}

void ARDOPModem::clearTNCPanel()
{
    // This one of few times the modem changes the TNC Panel...
    // Most of the status changes will be shown from the
    // state machine and widgets on the Panel itself
    // (like the waterfall and data scope).
    TNC->setWindowTitle(tr("ARDOP TNC Panel"));
    TNC->ui->ledSND->setEnabled(false);
    TNC->ui->lblTCP->setText("Connecting...");
    QString sheet = "QLabel {background-color: #ededed; }"; // ColorSystemButton;
    TNC->ui->lblTCP->setStyleSheet(sheet);
    TNC->ui->ledTCP->setEnabled(false);
    TNC->ui->lblRxFrameTxt->setText("");
    sheet = "QLabel {background-color: white; }"; // ColorSystemButton;
    TNC->ui->lblRxFrameTxt->setStyleSheet(sheet);
    TNC->ui->lblTxFrameTxt->setText("");
    TNC->ui->lblTxFrameTxt->setStyleSheet(sheet);
    TNC->ui->ledBZY->setEnabled(false);
    TNC->ui->ledCAR->setEnabled(false);
    TNC->ui->ledChg->setEnabled(false);
    TNC->ui->ledConn->setEnabled(false);
    TNC->ui->ledIRS->setEnabled(false);
    TNC->ui->ledISS->setEnabled(false);
    TNC->ui->ledTx->setEnabled(false);
    TNC->ui->ledRx->setEnabled(true);
}

void ARDOPModem::readSettings()
{
    QDir dir;
    QSettings settings(dir.canonicalPath() + "/ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("NETWORK");
      // general settings to local vars here
      mp->id = settings.value("DefaultNetworkConfiguration").toString();
      MCB->TCPAddress = settings.value("Address","127.0.0.1").toString();
      MCB->TCPPort    = settings.value("TCPPort",8515).toInt();
    settings.endGroup();

    settings.beginGroup("SOUND");
      MCB->CaptureDevice = settings.value("CaptureDevice","default").toString();
      MCB->PlaybackDevice = settings.value("PlaybackDevice","default").toString();
    settings.endGroup();

    settings.beginGroup("STATION");
      MCB->Callsign = settings.value("MyCall","unknown").toString();
      MCB->GridSquare = settings.value("GridSquare","XX00xx").toString();
//      MCB->Registration = settings.value("RegistrationNum","unreg").toString();
    settings.endGroup();
    settings.beginGroup("TNC");
      // general settings to local vars here
      MCB->CWID = settings.value("CWId",false).toBool();
      MCB->DebugLog = settings.value("DebugLog",false).toBool();
      MCB->ARQBandwidth = settings.value("ARQBandWidth",500).toInt();
      MCB->TxDriveLevel = settings.value("DriveLevel",50).toInt();
    settings.endGroup();
}

void ARDOPModem::saveSettings()
{
    // use TNC setup menu to save most settings ... not here
    // only temporary network or other interface settings
    // are saved here.
    QDir dir;
#ifdef DEBUG_MODEM
    qDebug() << dir.canonicalPath() + "/ardop_tnc.ini";
#endif
    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("NETWORK");
      settings.setValue("TCPIPPort", MCB->TCPPort);
      settings.setValue("TCPIPAddress", MCB->TCPAddress);
      settings.setValue("HostTCPIP", MCB->HostTCPIP);
      settings.setValue("DefaultNetworkConfiguration", mp->id);
    settings.endGroup();
    settings.sync();
}


void ARDOPModem::socketError(QAbstractSocket::SocketError err, QString errstr)
{
    Q_UNUSED(err)
    Q_UNUSED(errstr)
    QString msg = "";
    QString msg2 = "";
    Q_UNUSED(msg)

    switch ( err )
    {
    case QAbstractSocket::AddressInUseError:
        msg2 = "Address in use";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        msg2 = "Datagram Too Large";
        break;
    case QAbstractSocket::HostNotFoundError:
        msg2 = "Host Not Found";
        break;
    case QAbstractSocket::NetworkError:
        msg2 = "General Network Error";
        break;
    case QAbstractSocket::OperationError:
        msg2 = "Invalid Network Operation";
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        msg2 = "Proxy Authentication Required";
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        msg2 = "Proxy Connection Closed";
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        msg2 = "Proxy Connection Refused";
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        msg2 = "Proxy Connection Timeout";
        break;
    case QAbstractSocket::ProxyNotFoundError:
        msg2 = "Proxy Not Found";
        break;
    case QAbstractSocket::ProxyProtocolError:
        msg2 = "Proxy Protocol Violation";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        msg2 = "Remote Host Closed";
        break;
    case QAbstractSocket::SocketAccessError:
        msg2 = "Socket Access Error";
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        msg2 = "Socket Address Not Available";
        break;
    case QAbstractSocket::SocketResourceError:
        msg2 = "Socket Resource Error";
        break;
    case QAbstractSocket::SocketTimeoutError:
        msg2 = "Socket Timeout";
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        msg2 = "Ssl Handshake Failure";
        break;
    case QAbstractSocket::SslInternalError:
        msg2 = "Generalized Ssl Internal Error";
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        msg2 = "Invalid Ssl User Data";
        break;
    case QAbstractSocket::TemporaryError:
        msg2 = "Temporary Error";
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        msg2 = "Unfinished Socket Operation";
        break;
    case QAbstractSocket::UnknownSocketError:
        msg2 = "Unknown Socket Error";
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        msg2 = "Unsupportted Socket Operation";
        break;
    default:
        break;
    }

#ifdef DEBUG_MODEM
    msg  = tr("Modem ");
    msg += objTCPIPServer->serverAddress().toString();
    msg += ":";
    msg += QString("%1").arg(objTCPIPServer->serverPort());
    qDebug() << msg;
    msg = tr("Connection Error . . .");
    qDebug() << msg2 << errstr;
#endif
}


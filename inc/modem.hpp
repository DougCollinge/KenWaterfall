#ifndef MODEM_HPP
#define MODEM_HPP
/*! \file modem.hpp */
#include <QtCore/QDebug>    // to log messages or display on console
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QMultiHash>
#include <QtCore/QTime>
#include <QtCore/QEvent>
#include <QtCore/QByteArray>
#include <QtGui/QTextTableFormat>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QWidget>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkSession>
#include <QtSerialPort/QSerialPort>

// global definitions of macros and variables
#include "ardop_global.hpp"

// forward declare Qt Classes
class QTimer;

// forward declare modem classes
class TNCPanel;

/*! \ingroup MODEM
 * \brief The basic interface to the modem
 * \details
 * Proposed generic host interface definition for the modem. This
 * interface is TCP/IP only. All operation of the modem is through
 * commands from the interface using TCPIP datagrams. No direct
 * calls are made to any backend function or process.
 *
 * This interface will start the TNCPanel when connected to  a
 * host. The connection completion will signal a response from
 * the modem to start the codec and TNC.
 *
 * The codec has a separate state machine from the modem and may
 * run in a separate thread from the TNC GUI panel. */
class ARDOPModem : public QObject
{
    Q_OBJECT   // so that we can use signals, slots, and events
    Q_PROPERTY(bool    m_hostConnected  READ isHostConnected)                        // Read only property
    Q_PROPERTY(quint32 m_tcpipport      READ getTCPIPPort     WRITE setTCPIPPort)
    Q_PROPERTY(quint32 m_serbaud        READ getSerialBaud    WRITE setSerialBaud)
    Q_PROPERTY(QString m_nickname       READ getNickName      WRITE setNickName)
    Q_PROPERTY(QString m_tcpipaddress   READ getTCPIPAddress  WRITE setTCPIPAddress)
    Q_PROPERTY(QString m_serialportname READ getSerialPortName WRITE setSerialPortName)
    Q_PROPERTY(QString m_paring         READ getParing        WRITE setParing)
    Q_PROPERTY(QString m_interfacetype  READ getInterfaceType WRITE setInterfaceType)

public:
    enum InterfaceType
    {
        TCPIP,
        SERIAL,
        BLUETOOTH
    };

    // used for initial receive testing...
    // later put in correct protocol states
    enum ReceiveState
    {
        SearchingForLeader,
        AcquireSymbolSync,
        AcquireFrameSync,
        AcquireFrameType,
        DecodeFrameType,
        AcquireFrame,
        DecodeFrame,
    };

    // Enum of ARQ Substates
    enum ARQSubStates
    {
        ISSConReq,
        ISSConAck,
        ISSData,
        ISSId,
        IRSConAck,
        IRSData,
        IRSfromISS, // indicates the new IRS can receive data but cannot send a BREAK until Data is received from the new ISS
        None
    };

    enum ProtocolState
    {
        OFFLINE,                    //!< Offline Ignore all signals and sound card input
        DISC,                       //!< Disconnected
        ISS,                        //!< ISS in normal (transmitting data) state
        IRS,                        //!< IRS in normal (receiving data) state
        IDLE,                       //!< ISS in idle state ...no transmissions) [Replaced old ISS IDLE state in version 0.5.0.0 and later]
        IRStoISS,                   //!< IRS during transition to ISS waiting for ISS's ACK from IRS's BREAK
        FECSend,                    //!< FEC Sending station
        FECRcv,                     //!< FEC Receiving station
    };

    enum WindowFunction             //!< Enum of spectrum window types
    {
        NoWindow,
        HannWindow
    };

    enum ARQBW
    {
        BW_200MAX,
        BW_500MAX,
        BW_1000MAX,
        BW_2000MAX,
        BW_200FORCED,
        BW_500FORCED,
        BW_1000FORCED,
        BW_2000FORCED
    };

    enum ModemMode
    {
        Initializing,          //!< creating the modem
        WaitingForAudio,       //!< initialized waiting for audio input
        WaitingForConnection,  //!< initialized waiting for host connection
        Disconnected,          //!< protocol disconnected state
        Connecting,            //!< connecting to host
        Ready,                 //!< initialized, audio good, host connected and radio ctrl not used (or radio used and radio interface connected)
        ModemError,            //!< error in modem - see errnum or errMsg
        MODECHG,               //!< modem is in transition to another protocol mode
        XMIT,                  //!< PTT is ON and output for transmit to audio
        RCV                    //!< PTT is OFF and audio input is listening
    };



    // Radio control block contains all Radio setup
    // parametes (saved to ini file)
    struct RadioControlBlock
    {
        bool    RadioControl;      //!< True to enable radio control
        bool    CtrlPortRTS;       //!< Enable RTS on Radio Com Port
        bool    CtrlPortDTR;       //!< Enable DTR on Radio Com Port
        bool    RadioTCPIP;        //!< Use TCP/IP control port
        bool    RadioSerial;       //!< Use separate Serial/USB control port
        bool    RadioBlueTooth;    //!< Use BlueTooth radio control
        bool    InternalSoundCard; //!< use internal sound card (Kenwood 590, Icom 7100, 7200, 7600, 9100)
        bool    InternalTuner;     //!< Some radios which support tuner
        bool    PTTRTS;            //!< Enable PTT Port RTS for PTT keying
        bool    PTTDTR;            //!< Enable PTT Port DTR for PTT keying
        quint16 Ant;               //!< 0 (no control) 1, or 2
        quint16 Filter;            //!< bandwidth in Hz, 0 implies no bandwidth control
        quint32 CtrlPortBaud;      //!< default to 4800
        quint32 Frequency;         //!< Frequency in integer Hz
        QString RigModel;          //!< Radio model from drop down list
        QString IcomAdd;           //!< Icom address (Hex 00 - FF)
        QString Mode;              //!< "USB", "USBD", "FM"
        QString CtrlPort;          //!< Radio COM port
        QString PTTPort;           //!< Serial port or "External" for SignaLink type or VOX interface
    }; // RadioControlBlock

    /*! \struct Connection
     * \details
     * Structure for ARQ connection. Holds all the needed info on the
     * current connection...                       */
    struct ARQConnection
    {
        bool    blnLastPSNPassed;               //!< the last PSN passed True for Odd, False for even.
        bool    blnInitiatedConnection;         //!< flag to indicate if this station initiated the connection
        quint8  bytSessionID;                   //!< Session ID formed by 8 bit Hash of MyCallsign  and strRemoteCallsign always set to &HFF if not connected.
        qint32  intBytesSent;                   //!< Outbound bytes confirmed by ACK and squenced
        qint32  intBytesReceived;               //!< inbound bytes Received and sequenced to host
        qint32  intReceivedLeaderLen;           //!< received leader length in ms. (0- 2550) RECEIVED by THIS station
        qint32  intReportedLeaderLen;           //!< reported leader length in ms. (0- 2550) REPORTED by REMOTE station
        qint32  intCalcLeader;                  //!< the computed leader to use based on the reported Leader Length
        qint32  intTotalSymbols;                //!< To compute the sample rate error
        qint32  intSessionBW;                   //!< bandwidth for the session as an int
        double  dblAvgPECreepPerCarrier;        //!< computed phase error creep
        QDateTime dttLastIDSent;                //!< date/time of last ID
        QDateTime dttSessionStart;              //!< session start timestamp
        QString strRemoteCallsign;              //!< remote station call sign
        QString strLocalCallsign;               //!< the call sign used for this station
    }; // Connection

    explicit ARDOPModem(QObject *parent=0);
    virtual ~ARDOPModem();

    // local storage and control for modem settings of all modes //
    ModemControlBlock *MCB;                    // this is the only object
//    RadioControlBlock *RCB;                    // this is the only object

    //      PROPERTY SETS AND GETS       //
    void    setTCPIPPort(quint32 port)         { m_tcpipport = port; }
    void    setSerialBaud(quint32 baud)        { m_serbaud = baud; }
    void    setTCPIPAddress(const QString ip)  { m_tcpipaddress = ip; }
    void    setSerialPortName(const QString n) { m_serialportname = n.trimmed(); }
    void    setParing(const QString paring)    { m_paring = paring; }
    void    setNickName(const QString nam)     { m_nickname = nam.trimmed(); }
    void    setInterfaceType(const QString typ);

    bool    isHostConnected()   { return (strTCPIPControlConnectionID != "") && (strTCPIPDataConnectionID != ""); }
    quint32 getTCPIPPort()      { return m_tcpipport; }
    quint32 getSerialBaud()     { return m_serbaud; }
    QString getParing()         { return m_paring.trimmed(); }
    QString getInterfaceType()  { return m_interfacetype.trimmed().toUpper(); }
    QString getNickName()       { return m_nickname; }
    QString getTCPIPAddress();
    QString getSerialPortName();
    //        END SETS AND GETS          //

    int errNo;
    QString errMsg;

    void sendTCPMessage(const QString &message);

    /*! \brief  Function to send a text command to the Host
     * \details
     * This is from TNC side as identified by the
     * leading "c:"   (Host side sends "C:")
     *
     * Subroutine to send a line of text
     * (terminated with <Cr>) on the command port...
     * All commands beging with "c:" and end with <Cr>
     * A two byte CRC appended following the <Cr>
     * for non TCPIP connections.
     *
     * The strText cannot contain a "c:" sequence
     * or a <Cr>.
     *
     * \returns TRUE if command sent successfully.
     * Form byte array to send with CRC
     *
     * \todo  Complete for Serial and BlueTooth */
    bool SendCommandToHost(QString);

    void disconnect();
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;
    QString nickName() const;


signals:
    void newInData(QString);
    void signalCtrlChgd(QString, QColor, QString, quint32); //!< \details Used to signal for TNC Panel display change

protected:
    void closeEvent(QCloseEvent *event);

public slots:

private slots:
    void slotConnected();
    void slotConnectionError(QAbstractSocket::SocketError);
    void slotUpdateClient(qint64);

    /*! \brief incomingConnection slot
     * \details
     * This slot is connected to the
     * newConnection() signal from QTcpServer.
     * Every new connection will be run in
     * a the current thread. Once a connection
     * is not needed, it will be be deleted later.
     * */
    void slotNewConnection();

    void slotNewMessage();
    void slotRemoveConnection(QString);
    void slotNetworkTimerTimeout();
    void slotSocketStateChg(QAbstractSocket::SocketState);
    void disconnected();
    void connectionError();
    void processReadyRead();
    void appendMessage(const QString &from, const QString &message);
    void showInformation();
    void sessionOpened();
    void updateAddresses();


private:
    //    property vars       //
    bool    m_hostConnected;
    quint32 m_tcpipport;
    quint32 m_serbaud;
    QString m_nickname;
    QString m_tcpipaddress;
    QString m_serialportname;
    QString m_interfacetype = "TCPIP";
    QString m_paring;
    //   end property vars    //

    enum NetworkState
    {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };

    enum DatagramType
    {
        PlainText,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    bool    blnClosing;

    QString myNickName;
    QString strLastCommandToHost;
    QString strTCPIPControlConnectionID;
    QString strTCPIPDataConnectionID;

    QMultiHash<QHostAddress, QTcpSocket *> peers;
    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;

    QNetworkConfiguration config;

//    TCPConnection objTCPIPCmdClient;
    NetworkState      connstate;
    ModemMode         modem_mode;
    InterfaceType     interface;

    QTcpServer        *objTCPIPServer;
    QTcpSocket        *objTCPIPCmdClient;
    QNetworkSession   *networkSession;

    class ARDOPModemPrivate;
    ARDOPModemPrivate *mp;                  //!<

    QTimer          *tmrNetworkSend;
//    QTimer          *tmrTCPListen;          //!< checks periodically for lost connection


    QSerialPort    *objSerial;

    TNCPanel       *TNC;

    void clearTNCPanel();
    void processData();
    void processCMD(QString cmd);
    void ProcessMessage(QString);
    void commandInterpreter(QString cmd);
    void removeConnection(QTcpSocket *);
    void sendMessage(const char*);
    void readSettings();
    void saveSettings();
    void socketError(QAbstractSocket::SocketError,QString);

    bool EnableHostLink();                  //!< Enable a TCPIP, Serial,  or BlueTooth Link with the Host (Opens the port for listening...Does NOT initiate the connection!)
    bool TerminateHostLink();               //!< Function to terminate the Host link
    bool readCMDHeader();
    bool hasEnoughData();


    int dataLengthForCurrentDataType();
    int readDataIntoBuffer(int maxSize);
};
#endif // MODEM_HPP

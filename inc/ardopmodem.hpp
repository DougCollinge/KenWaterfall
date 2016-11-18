#ifndef ARDOPMODEM_HPP
#define ARDOPMODEM_HPP
/*! \file modem.hpp */
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QTime>
#include <QtCore/QByteArray>
#include <QtGui/QTextTableFormat>
#include <QtCore/QEvent>
#include <QtGui/QCloseEvent>

#include "xmlrpcclient.hpp"

//for global definitions of macros and variables
#include "ardop_global.hpp"

class QTcpSocket;
class QNetworkReply;
class QSslError;

class TNCPanel;
class TNCSetup;

namespace modem {
class ARDOPModem;
}

/*! \ingroup MODEM
 * \class ARDOPModem
 * \brief proposed generic modem interface definition
 * \details  */
class ARDOPModem : public QObject
{
    Q_OBJECT   // so that we can use signals, slots, and events

public:
    ARDOPModem();
    virtual ~ARDOPModem();

    enum ConnectionState
    {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };


    int errNo;
    QString errMsg;

    QString nickName() const;

signals:
    void newInData(QString);

protected:

public slots:

private slots:
    void sendGreetingMessage();

private:
    // ModemControlBlock contains all
    // TNC setup parameters (saved to ini file)
    // and control parameters for operation
    struct ModemControlBlock
    {
        bool    CodecStarted;
        bool    Stereo;
        bool    HostTCPIP;
        bool    HostSerial;
        bool    HostBlueTooth;
        bool    CWID;
        bool    DebugLog;
        bool    StartMinimized;
        bool    CommandTrace;
        bool    AccumulateStats;
        bool    DisplayWaterfall;
        bool    DisplaySpectrum;
        bool    SecureHostLogin;
        bool    LinkedToHost;
        bool    FECId;
        bool    SlowCPU;
        bool    PTT;              //!< modem PTT flag for all methods

        quint16 RxAudioSampleSize;
        quint16 RxSquelchLevel;
        quint16 BusyDet;
        quint16 RxDataFrameSize;
        quint16 RxVolume;
        quint16 TxAudioSampleSize;
        quint16 TxDriveLevel;
        quint16 TxDataFrameSize;
        quint16 LeaderLength;
        quint16 TrailerLength;
        quint16 TuningRange;
        quint16 FECRepeats;

        quint32 RxAudioSampleRate;
        quint32 CaptureClock;
        quint32 TxAudioSampleRate;
        quint32 PlaybackClock;
        quint32 SerBaud;

        qint16  TCPPort;

        quint32 ARQConReqRepeats;
        quint32 ARQTimeout;

        double  DisplayFreq;

        QString Callsign;
        QString GridSquare;
        QString CaptureDevice;
        QString PlaybackDevice;
        QString TCPAddress;
        QString SerCOMPort;
        QString HostPairing;
        QString ARQBandwidth;
        QString ProtocolMode;
        QString Password;
        QString FECMode;
        QStringList AuxCalls;
    };  //ModemControlBlock
    static ModemControlBlock *MCB;          //!< This is the only copy. Pass the reference pointer from here.

    class ModemPrivateData;
    ModemPrivateData *d;

    void removeConnection(QTcpSocket *);
    void processData();
    void doClient();
    void processCMD(QString cmd);
    void sendMessage(const char*);
    void handleConnErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void commandInterpreter(QString cmd);
    void readSettings();
    void saveSettings();

    bool readProtocolHeader();
    bool hasEnoughData();

    int readDataIntoBuffer(int maxSize);
};

#endif // ARDOPMODEM_HPP

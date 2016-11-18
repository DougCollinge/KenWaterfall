#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QPixmap>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>

#include "ardop_global.hpp"
// includes for codecs go here  //
// connects to public interface of the back end codec
#include "tncpanel.hpp"
#include "tncsetup.hpp"

#include "ardopmodem.hpp"

class ARDOPModem::ModemPrivateData
{
public:
    int m_buffers;         // number of buffers set or returned
    int m_bw;              // bandwidth size set or returned
    int bytesToWrite;
    int bytesWritten;
    int bytesReceived;
    int bytesToRcv;
    int bytesAvailable;
    int transferTimerId;
    int TotalBytes;
    int numBytesForCurrentDataType;

    quint16 blockSize;

    QString id;
    QString myNickName;
    QString username;
    QString greetingMessage;
    QString m_grid;        // gridsquare set or returned

    XmlRpcClient *host;
};

ARDOPModem::ARDOPModem() : QObject()
{
    if ( MCB->DebugLog ) qDebug() << "Constructor Starting... ";

    d = new ModemPrivateData;
    d->id = "";
    readSettings();  // get stored settings for modem use
    // Get saved network configuration
    QString tcp = MCB->TCPAddress;
    quint32 port = MCB->TCPPort; // data port is port+1
    QString svrstr = "http://" + tcp  + QString("%1").arg(port);
    d->host = new XmlRpcClient(svrstr,this);
    if ( MCB->DebugLog ) qDebug() << "Host ID " + tcp + " : " + port;
    QSslConfiguration config = d->host->getConfig();
    config.setProtocol(QSsl::AnyProtocol);
    d->host->setSslConfiguration(config);
    connect(d->host,SIGNAL(sslErrors(QNetworkReply *,const QList<QSslError> &)),this, SLOT(handleConnErrors(QNetworkReply *, const QList<QSslError> &)));
    doClient();
}

ARDOPModem::~ARDOPModem()
{
    //
    if ( MCB->DebugLog ) qDebug() << "...Modem closing";
    delete d->host;
    delete d;
}

// =========================================================
// PUBLIC METHODS
// =========================================================

// =========================================================
// PRIVATE FUNCTIONS
// =========================================================
void ARDOPModem::processCMD(QString cmd)
{
    qDebug() << "Processing CMD: " + cmd;
}

void ARDOPModem::doClient()
{
    QVariantList args;
    /*
    d->host->call("examples.nix", args,this, SLOT(testResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));
    args << QVariant(7);

    d_data->rpc->call("examples.getStateName", args,this, SLOT(testResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));
    d_data->rpc->call("examples.birne", args,this, SLOT(testResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));
    args[0] = QVariant(-128);

    d_data->rpc->call("examples.birne", args,this, SLOT(testResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));
    d_data->rpc->call("examples.notfound", args,this, SLOT(testResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));

    QDateTime towelDay;
    towelDay.setDate(QDate(2008, 5, 25));
    */
    args.clear();
//    args << towelDay;
//    d->host->call("examples.plusOneYear", args,this, SLOT(towelResponse(QVariant &)),this, SLOT(testFault(int, const QString &)));
}


void ARDOPModem::testResponse(QVariant &arg)
{
    if ( MCB->DebugLog ) qDebug() << arg.toString();
}

void ARDOPModem::testFault(int error, const QString &message)
{
    if ( MCB->DebugLog ) qDebug() << "EEE:" << error << "-" << message;
}

void ARDOPModem::towelResponse(QVariant &arg)
{
    if ( MCB->DebugLog ) qDebug() << "Next years Towel Day is on" << arg.toDateTime();
}


void ARDOPModem::sendMessage(const char *message)
{
    /*
    if ( MCB->DebugLog ) qDebug() << "got " + QString(message).toUtf8();
    QByteArray msg = QString(message).toUtf8();
    QByteArray data = "MESSAGE " + ' ' + msg;
    if ( MCB->DebugLog ) qDebug() << "Modem sending message " + data;
    if ( MCB->DebugLog ) qDebug() << "Modem port to host open for write: " + boolTcpPortOpen;
    */
}

void ARDOPModem::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    if ( MCB->DebugLog ) qDebug() << "SSL Error:" << errors;
    reply->ignoreSslErrors(); // don't do this in real code! Fix your certs!
}

QString ARDOPModem::nickName() const
{
    QString strMsg;
    strMsg = MCB->TCPAddress;
    strMsg += '@';
    strMsg += d->host->getClientName(); //TODO: use getClientName()
    strMsg += ':';
    strMsg += QString("%1").arg(MCB->TCPPort);
    return strMsg;
}

void ARDOPModem::commandInterpreter(QString cmd)
{
    // start up the appropriate TNC or pass settings to codec
    QList<QString> incmd = cmd.split(" ",QString::KeepEmptyParts);
    QString msgstr = "Interpreting CMD - " + incmd[0] + " " + incmd[1];
    qDebug() << msgstr;
    if ( incmd[0] == "ABORT" )
    {
        // dirty disconnect from any mode
        // decypher mode
        //   decypher sub-mode
    }
    // not mode so decypher which command
    else if ( incmd[0] == "ARQBW" )
    {
        // Set/gets the bandwidth for ARQ mode.
        // - determine bandwidth from second param
        // - check against stored vector ARQBandwidths
        // - set modem control block to new sub-mode
        // - update TNC with new sub-mode.
        // This sets the maximum negotiated bandwidth or
        // sets the forced bandwidth to a specific value.
        // Attempting to change bandwidth while a
        // connection is in process will generate a FAULT.
        // If no parameter is given will return the
        // current bandwidth setting. This bandwidth
        // setting applies to all call signs used
        // (MYCALL plus optional call signs MYAUX)
    }
    else if ( incmd[0] == "ARQTIMEOUT" )
    {
        // Set/get the ARQ Timeout in seconds (30/60).
        // BACKEND ACTION:
        // If no data has flowed in the channel in
        // ARQTimeout seconds the link is declared dead.
        // A DISC command is sent and a reset to the
        // DISC state is initiated. If either end of
        // the ARQ session hits it’s ARQTIMEOUT without
        // data flow the link will automatically be terminated.
    }
    else if ( incmd[0] == "ARQCALL")
    {
        // Ask for ARQ connect with num of repeats.
        // Repeat count must be 2 to 15.
        // Target Call sign must be a legitimate call
        // sign syntax or “CQ” e.g. ARQCALL W1AW 5
    }
    else if ( incmd[0] == "AUTOBREAK" )
    {
        // Disables/enables automatic link turnover
        // (BREAK) by IRS when IRS has outbound
        // data pending and ISS reaches IDLE state.
        // Default = True.
        qDebug() << "AutoBreak rcvd";
        if ( incmd.size() == 1 )
        {
            sendMessage("AUTOBREAK" + MCB->AutoBREAK );
            return;
        }
        if ( incmd.size() > 1 ) MCB.AutoBREAK = false;    // if value is changing reset
        if ( incmd[1] == "true" ) MCB.AutoBREAK = true;  // set value to value sent
        // decypher mode
    }
    else if ( incmd[0] == "BREAK" )
    {
        // Initiates a BREAK
        // BACKEND ACTION:
        // (link turnover request to the ISS)
        // if in IRS state otherwise it has
        // no effect. Forces ISS to clear its
        // outbound queue and acknowledge with ACK.
        // Normally not required if AUTOBREAK is enabled.
        qDebug() << "BREAK rcvd";
        // decypher mode
    }
    else if ( incmd[0] == "BUFFER" )
    {
        // Send signal to TNC to report current
        // buffer size.
        // BACKEND ACTION:
        // Gets the current outbound data buffer
        // size in bytes. BUFFER is also sent
        // asynchronously whenever there is a
        // change in the outbound buffer size.
        // The reported size includes any data
        // that is currently in process
        // (being sent but not yet acknowledged
        // received)
        //
        // Asynchronous Response:
        // Reply format is:
        // BUFFER <data out queued>
        // Value is in integer bytes. BUFFER may
        // also be polled using the BUFFER command
        // with no parameters. BUFFER value
        // includes any data frame currently being
        // sent but not yet ACKed by the remote IRS.
    }
    else if ( incmd[0] == "BUSY" )
    {
        // <TRUE/FALSE>
        // FALSE param = Clear channel detected
        // TRUE  param = Busy channel detected
    }
    else if ( incmd[0] == "BUSYLOCK" )
    {
        // <TRUE/FALSE>
        // Disable/Enable Busy channel blocking.
        // Busyblock will block a connection request
        // unless there have been T quiet ms of
        // non-busy status preceding the connect
        // request. (See appendix B of protocol docs)
        // Default = False
    }
    else if ( incmd[0] == "BUSYDET" )
    {
        // <1-10>
        // Returns or sets the current
        // Busy detector threshold value (default = 5).
        // The default value should be
        // sufficient for most installations.
        // BUSYDET affects the sensitivity of the
        // busy detector (low values = higher
        // sensitivity but increased false triggering).
    }
    else if ( incmd[0] == "CANCELPENDING" )
    {
        // Aysynchronous reply (read only)
        // Indicates to the host that the prior
        // PENDING Connect Request was not to
        // MYCALL or one of the MYAUX call
        // signs) This allows the Host to
        // resume scanning.
    }
    else if ( incmd[0] == "CAPTURE" )
    {
        // <capture device name>
        // Sets desired sound card capture device.
        // If no device name TNC will reply with the
        // current assigned capture device from settings.
    }
    else if ( incmd[0] == "CAPTUREDEVICES" )
    {
        // Returns a comma delimited list of
        // all currently discovered capture devices
        // from TNC audio component.
    }
    else if ( incmd[0] == "CONNECTED" )
    {
        // Asynchronous reply (read only)
        // <remote Call Bandwidth in Hz>
        // e.g. “CONNECTED W1ABC 500”
        // An ARQ connection has been established.
        // <remote Call> contains the connected call
        // sign with a negotiated bandwidth. This
        // follows the TARGET <target call sign>
        // asynchronous response if the requested
        // bandwidths are compatible and the
        // connection negation is complete.
    }
    else if ( incmd[0] == "CLOSE" )
    {
        // Provides an orderly shutdown of
        // all connections, release of all
        // sound card resources and closes
        // the Virtual TNC Program or hardware.
    }
    else if ( incmd[0] == "CMDTRACE" )
    {
        // <TRUE/FALSE>
        // BACKEND ACTION:
        // Get/Set Command Trace flag to log
        // all commands to or from the TNC to
        // the ARDOP TNC debug log.
    }
    else if ( incmd[0] == "CODEC" )
    {
        // <TRUE/FALSE>
        // Start the Codec with True, Stop
        // with False. No parameter will
        // return the Codec state.
    }
    else if ( incmd[0] == "CWID" )
    {
        // <TRUE/FALSE>
        // Disable/Enable the CWID option.
        // BACKEND ACTION:
        // CWID is optionally sent at the
        // end of each ID frame. This
        // option is stored in the modem settings
    }
    else if ( incmd[0] == "DATATOSEND")
    {
        // <0>
        // BACKEND ACTION:
        // If sent with the parameter 0 (zero)
        // it will clear the TNC’s data to send
        // Queue. Cleared data can be recovered
        // using the RESTORBUFFER command. If
        // sent without a parameter, will return
        // the current number of data bytes queued
        // to send.
    }
    else if ( incmd[0] == "DEBUGLOG" )
    {
        // Enable/disable the debug log.
        // With the log enabled, the debug and other
        // application messages will be sent to
        // a selected log file.
        // When disabled, messages are sent to the
        // application console.
    }
    else if ( incmd[0] == "DISCONNECT" )
    {
        // Initiates a normal disconnect cycle
        // for an ARQ connection. If not
        // connected, command is ignored.
    }
    else if ( incmd[0] == "DISCONNECTED" )
    {
        // Aysynchronous reply (read only)
        // An existing ARQ link has been disconnected
    }
    else if ( incmd[0] == "DISPLAY" )
    {
        // <Frequency in Khz>
        // Sets the Dial frequency display
        // of the Waterfall or Spectrum display
        // on the TNC Display Panel.
        // If sent without parameters will
        // return the current Dial frequency
        // display. If > 100000 Display will
        // read in MHz.
    }
    else if ( incmd[0] == "DRIVELEVEL" )
    {
        // <0-100>
        // Set Drive level. Default = 100 (max)
        quint32 m_driveLvl;
        m_driveLvl = incmd.at(1).toInt();
        MCB->TxDriveLevel = m_driveLvl;
        // process for mode
    }
    else if ( incmd[0] == "FAULT" )
    {
        // Asynchronous reply (read only)
        // A program fault or error condition.
    }
    else if ( incmd[0] == "FECID" )
    {
        // Disable/Enable ID
        // (with optional grid square)
        // at start of FEC transmissions
    }
    else if ( incmd[0] == "FECMODE" )
    {
        // BACKEND ACTION:
        // Sets the modulation mode and bandwidth
        // for FEC (broadcast/multicast) transmission.
        // Details on the specific frame types can
        // be found in the ARDOP frame type spreadsheet.
        // In general the first component of the frame
        // ID is the modulation type e.g. 4FSK, 8PSK
        // etc. The second is the bandwidth in Hz (@-26 dB).
        // The third is the baud rate. Some modes
        // also allow a shortened frame designated by
        // a trailing “S”. The above are in order
        // of robustness for most channel types.
        // Note: baud rates over 300 are not currently
        // permitted in the US below 29 MHz.
        // Mode saved in MCB settings.
    }
    else if ( incmd[0] == "FECREPEATS" )
    {
        // <0-5>
        // Sets the number of times a frame
        // is repeated in FEC (multicast)
        // mode. Higher number of repeats
        // increases good copy probability
        // under marginal conditions but
        // reduces net throughput.
    }
    else if ( incmd[0] == "FECSEND" )
    {
        // <TRUE/FALSE>
        // BACKEND ACTION:
        // Start/Stop FEC broadcast/multicast
        // mode for specific FECMODE. FECSEND
        // <False> will abort a FEC broadcast.
    }
    else if ( incmd[0] == "FREQUENCY" )
    {
        // Asynchronous reply (read only)
        // If TNC Radio control is enabled the FREQUENCY
        // response is sent to the Host upon a change in
        // frequency of the radio. This can be caused
        // by either a new programmed frequency, a Dial
        // turn on radios with frequency read back or
        // a mouse click on the Waterfall or Spectrum
        // displays (in USB or USBD modes causing a
        // retune). Does not return the new frequency
        // when the command RADIOFREQ sent to the
        // modem. The frequency reported is the DIAL
        // frequency of the radio.
    }
    else if ( incmd[0] == "GRIDSQUARE" )
    {
        // <4,6, or 8 char grid locator>
        // BACKEND ACTION:
        // Sets or retrieves the 4, 6, or 8 character
        // Maidenhead grid square (used in ID Frames)
        // for the user station location.
        // An improper grid square syntax will return a FAULT.
        // Grid is stored in modem settings.
    }
    else if ( incmd[0] == "INITIALIZE" )
    {
        // Clears any pending queued values
        // in the TNC interface. Should be sent
        // upon initial connection and before
        // any other parameters are sent.
    }
    else if ( incmd[0] == "ISREGISTERED" )
    {
        // not currently used
    }
    else if ( incmd[0] == "LEADER" )
    {
        // <100-2000>
        // BACKEND ACTION:
        // Get/Set the leader length in ms.
        // (Default is 240 ms). Rounded to
        // the nearest 20 ms.
        // Note: For ARQ connections leader
        // length is automatically adjusted
        // based on the leader reported
        // received by the remote station.
    }
    else if ( incmd[0] == "LISTEN" )
    {
        // <TRUE/FALSE>
        // Enables/disables server’s response
        // to an ARQ connect request to MYCALL
        // or any of MYAUX call signs.
        // Default = TRUE.
        // May be used to block connect requests
        // during scanning or periods when
        // server is offline or in the process
        // of changing frequency.
    }
    else if ( incmd[0] == "MONITOR" )
    {
        // <TRUE/FALSE>
        // Enables/disables monitoring of
        // FEC or ARQ Data Frames, ID frames,
        // or Connect request in disconnected
        // ARQ state. Default=TRUE
    }
    else if ( incmd[0] == "MYAUX" )
    {
        // Sets up to 10 auxiliary call signs
        // that will answer ARQ connect requests.
        // Call signs must be valid radio call
        // signs and separated by commas. If
        // sent with an illegal call sign
        // (e.g. “MYAUX x” it will clear the
        // MYAUX list. If sent without a parameter
        // will return a comma delimitate string
        // of current MYAUX call signs.
        // Legitimate call signs include from
        // 3 to 7 ASCII characters (A-Z, 0-9) followed
        // by an optional “-“ and an SSID of
        // -0 to -15 or -A to -Z. An SSID of
        // -0 is treated as no SSID.
        // Aux callsigns are stored in the settings.
    }
    else if ( incmd[0] == "MYCALL" )
    {
        // <CALLSIGN>
        // Sets current call sign.
        // If not a valid call generates
        // a FAULT. Legitimate call signs
        // include from 3 to 7 ASCII
        // characters (A-Z, 0-9) followed
        // by an optional “-“ and an
        // SSID of -0 to -15 or -A to -Z.
        // An SSID of -0 is treated as no SSID.
    }
    else if ( incmd[0] == "NEWSTATE" )
    {
        // Asynchronous reply (read only)
        // reports any protocol state change.
        // Reply options:
        // < OFFLINE|DISC|ISS|QUIET|IRS|IRStoISS|FECSend|FECRcv>
    }
    else if ( incmd[0] == "PENDING" )
    {
        // Asynchronous reply (read only)
        // Indicates to the host application a
        // Connect Request frame type has been detected
        // (may not necessarily be to MYCALL or one
        // of the MYAUX call signs). This provides
        // an early warning to the host that a
        // connection may be in process so it can
        // hold any scanning activity.
    }
    else if ( incmd[0] == "PLAYBACK" )
    {
        // Sets desired sound card playback device.
        // If no device name will reply with the
        // current assigned playback device.
    }
    else if ( incmd[0] == "PLAYBACKDEVICES" )
    {
        // Returns a comma delimited list of
        // all currently detected playback devices
        // from the audio component of the TNC.
    }
    else if ( incmd[0] == "PROTOMODE")
    {
        // BACKEND ACTION:
        // Sets/Gets the protocol mode.
        // If ARQ and LISTEN above are TRUE,
        // will answer Connect requests to
        // MYCALL or any call signs in MYAUX.
        // If FEC will decode but not respond
        // to any connect request.
    }
    else if ( incmd[0] == "PTT" )
    {
        // Asynchronous reply (read only)
        // Indicates to the host application to
        // key the PTT on (PTT True) or off (PTT False)
        // To operate correctly the transmitter PTT
        // should be activated within 50 ms of receipt
        // of this response. Excessive delay in PTT
        // application or removal may cause a failure
        // in ARQ modes if measured latency exceeds 250 ms.
        // NOTE:
        // Not sent if Radio Control PTT is locally initiated by
        // the TNC.
    }
    else if ( incmd[0] == "PURGEBUFFER" )
    {
        // Clears any data in the outbound buffer.
        // Should precipitate a “BUFFER 0” asynchronous
        // response. Data purged goes into a
        // TNC buffer that can be restored to
        // the outbound buffer (outbound Queue)
        // with a RESTOREBUFFER command.
    }
    // ------------------------------------------
    // The following RADIO commands support
    // optional radio control that may be used in
    // some applications (e.g. Virtual TNC is
    // running on a remote computer located with the
    // radio). All radio commands begin with “RADIO”.
    // ------------------------------------------
    else if ( incmd[0] == "RADIOANT" )
    {
        // Selects the radio antenna 1 or 2
        // for those radios that support
        // antenna switching. If the parameter
        // is 0 will not change the antenna
        // setting even if the radio supports it.
        // If sent without a parameter will
        // return 0, 1 or 2. If RADIOCONTROL
        // is FALSE or RADIOMODEL has not been
        // set will return FAULT
    }
    else if ( incmd[0] == "RADIOCTRL" )
    {
        // Enables/disables the radio control
        // capability of the ARDOP TNC. If
        // sent without a parameter will return
        // the current value of RADIOCONTROL enable.
    }
    else if ( incmd[0] == "RADIOCTRLBAUD" )
    {
        // <1200-115200>
        // (Note: baud rates >4800 recommended for PTT
        // control due to T>R Latency)
    }
    else if ( incmd[0] == "RADIOCTRLDTR" )
    {
        // Enable/disable DTR Line on Control port.
    }
    else if ( incmd[0] == "RADIOCTRLPORT" )
    {
        // Set/get the radio control com port
        // to use for radio control.
    }
    else if ( incmd[0] == "RADIOCTRLRTS" )
    {
        // Enable/disable RTS Line on Control port.
    }
    else if ( incmd[0] == "RADIOFLTR" )
    {
        // <0,1,2,3>
        // This sets (for selected radios)
        // the pre setup filter for the radio.
        // A value of 0 will disable filter
        // control. Values 1, 2, or 3 will
        // setup the first second or
        // third filters in the radio.
        // Some radios will not support 3
        // filters. Filters should be setup on
        // the radio to be centered on 1500 Hz
        // and the appropriate bandwidth
        // for the requested mode Bandwidth.
        // 0 disables filter control.
    }
    else if ( incmd[0] == "RADIOFREQ" )
    {
        // If Radio Control is enabled in the
        // ARDOP modem, sets the Dial frequency
        // of the radio and the display of the
        // Waterfall or Spectrum display of the
        // TNC Panel. If sent without parameters
        // will return the current Dial frequency
        // of the radio.
    }
    else if ( incmd[0] == "RADIOCOMADD" )
    {
        // Sets/reads the current Icom Address
        // for radio control (Icom radios only).
        // Values must be hex 00 through FF
    }
    else if ( incmd[0] == "RADIOINC" )
    {
        // Enable/Disable Radio’s internal
        // sound card (some radios)
    }
    else if ( incmd[0] == "RADIOMENU" )
    {
        // Enable/Disable the Radio menu
        // item on the TNC Panel.
    }
    else if ( incmd[0] == "RADIOMODE" )
    {
        // Sets the radio modulation mode
        // to USB, USB Digital (some radios)
        // or FM (some radios). If sent
        // without a parameter will return the
        // current value of RADIOMODE.
    }
    else if ( incmd[0] == "RADIOMODEL" )
    {
        // If radio control is enabled accepts
        // the radio model. If sent without a
        // parameter returns the current radio
        // model. If radio not supported returns FAULT
    }
    else if ( incmd[0] == "RADIOMODELS" )
    {
        // Returns a comma delimited list
        // of supported radio models from
        // the RadioModels vector list.
    }
    else if ( incmd[0] == "RADIOPTT" )
    {
        // <CATPTT/VOX,VOX/SIGNALINK,COMn>
        // Selects CATPTT,VOX(SignaLink) mode
        // or COM port for PTT Control
    }
    else if ( incmd[0] == "RADIODTRPTT" )
    {
        // Enables/disables PTT keying
        // using DTR signal on RADIOPTT Com port
    }
    else if ( incmd[0] == "RADIORTSPTT" )
    {
        // Enables/disables PTT keying
        // using RTS signal on RADIOPTT Com port
    }
    else if ( incmd[0] == "RADIOTUNER" )
    {
        // Enable/Disable internal radio tuner
        // (some radios). (not currently implemented)
    }
    // ------------ END OF RADIO CMDS ---------------
    else if ( incmd[0] == "RESTOREBUFFER" )
    {
        // Restores data from the most recent PURGEBUFFER
        // command. Should precipitate an asynchronous
        // “BUFFER nnnn” response.
    }
    else if ( incmd[0] == "REJECTEDBW" )
    {
        // Asynchronous reply (read only)
        // <Remote Call sign>
        // Used to signal the host that a connect
        // request to or from Remote Call sign was
        // rejected due to bandwidth incompatibility
    }
    else if ( incmd[0] == "REJECTEDBUSY" )
    {
        // Asynchronous reply (read only)
        // <Remote Call sign>
        // Used to signal the host that a connect request
        // to/from Remote Call sign was rejected due
        // to channel busy detection.
    }
    else if ( incmd[0] == "SENDID" )
    {
        // BACKEND ACTION:
        // This will send an ID frame and
        // if CWID above is enabled followed
        // by a FSK CW ID.
        // The protocol must be in the DISC
        // protocol state or a <Fault: Not from state...>
        // will be replied.
    }
    else if ( incmd[0] == "SETUPMENU" )
    {
        // Enabled/Disable the Setup Menu
        // on the ARDOP TNC panel.
    }
    else if ( incmd[0] == "SQUELCH" )
    {
        // Returns or sets the current squelch
        // value (default = 5). The default value
        // should be sufficient for most installations.
        // Squelch affects the sensitivity of the
        // leader detector (low values = higher
        // sensitivity but increased false triggering).
        // Not to be confused with radio squelch setting
        // which is a separate hardware adjustment.
    }
    else if ( incmd[0] == "STATE" )
    {
        // <OFFLINE|DISC|ISS|IRS|IRStoISS|QUIET|FECSend|FECRcv)>
        // (READONLY)
        // Gets the current ARDOP protocol state
        // Every State change is also reported asynchronously
        // with the NEWSTATE reply below.
    }
    else if ( incmd[0] == "STATUS" )
    {
        // Asynchronous reply (read only)
        // Used to signal information text to user.
        // e.g. “STATUS CONNECT WITH W1AW FAILED”.
        // The syntax of STATUS commands is not
        // rigidly controlled so if used should be
        // for user display purposes only.
    }
    else if ( incmd[0] == "TARGET" )
    {
        // Asynchronous reply (read only)
        // <target call>
        // Identifies the target call sign of
        // the connect request. The target call
        // will be either MYCALL or one of the
        // MYAUX call signs.
    }
    else if ( incmd[0] == "TRAILER" )
    {
        // Get/Set the trailer length in ms.
        // (Default is 0 ms). Rounded to the
        // nearest 20 ms. Normally not required
        // except for some SDR Radios.
    }
    else if ( incmd[0] == "TUNE" )
    {
        // Asynchronous reply (read only)
        // <Tuning offset in integer Hz>
        // Sent when the waterfall or spectrum is clicked
        // and can be used by the host to adjust
        // radio settings. This is bounded to
        // approx. +/- 1200 Hz. If radio control
        // is enabled will also tune the radio
        // to the new frequency.
    }
    else if ( incmd[0] == "TUNERRANGE" )
    {
        // Get/set the DSP tuning range in Hz.
        // 0 should only be used in FM or AM
        // radio connections.
    }
    else if ( incmd[0] == "TWOTONETEST" )
    {
        // Send 5 second two-tone burst at
        // the normal leader amplitude. May be used in
        // adjusting drive level to the radio.
        // If sent while in any state except
        // DISC will result in a FAULT “not from state .....”
    }
    else if ( incmd[0] == "VERSION" )
    {
        // Returns the name and version of
        // the ARDOP modem program or hardware
        // implementation.
    }
    else if ( incmd[0] == "VOX" )
    {
    }
    // Data transfer:
    //
    // Data is transferred on the next sequential
    // TCP address/port, or the same Serial COM port
    // or Bluetooth pairing as commands using binary
    // byte transfers with a specific header
    // defined below.
    // The format for data going from the Host to the TNC is:
    // ”D:” +2 byte count (Hex 0001 – FFFF most significant byte
    // first), binary data,+2 Byte CRC”. For TCPIP interfaces
    // the 2 Byte CRC is not used since CRC and repeats are
    // integral to the TCPIP protocol. The “D:”
    // identifies this as a data frame from the Host.
    // Unlike earlier versions no RDY response is
    // issued upon a successful decode. All data frames are
    // buffered and processed in a first in first out basis.
    // If the TNC received the data incorrectly it replies a
    // CRC failure
    // “c:CRCFAIL<Cr> + 2 byte CRC). (No CRC on TCPIP
    // interface commands) The TNC will buffer this data in
    // its outbound queue until transmitted. A failed CRC should
    // prompt the Host to resend the frame up to 3 times
    // before declaring an interface fault.
    // The format for data going from the TNC to the Host is:
    // ”d:” + 2 byte count (Hex 0001 – FFFF most significant byte
    // first), + “ARQ|FEC|ERR|IDF”, + binary data, +2 Byte CRC”.
    // For TCPIP interfaces the 2 Byte CRC is not used since
    // CRC and repeats are integral to the TCPIP protocol.
    // The “d:” identifies this as a data frame from the TNC. ”ARQ”
    // indicates this is data received from a connected (ARQ)
    // session and should be error free.
    // ”FEC” indicates this is data received from an unconnected
    // session ( a monitored FEC or monitored ARQ frame) and
    // should be error free. ERR indicates this is Data received
    // from an unconnected session which failed CRC check of
    // the received frame . It can be masked or ignored by the
    // Host if desired. ”IDF” indicates this is ID frame information
    // If the Host received the data incorrectly a CRC failure
    // “C:CRCFAIL<Cr> + 2 byte CRC). The TNC will then respond
    // with the next data frame or a repeat of the failed data
    // frame. For TCPIP interfaces the 2 Byte CRC is not used
    // since CRC and repeats are integral to the TCPIP protocol.
}

void ARDOPModem::readSettings()
{
    QSettings settings("OpenModem","Config");
    settings.beginGroup("Sound");
      MCB->CaptureDevice = settings.value("CaptureDevice","default").toString();
      MCB->PlaybackDevice = settings.value("PlaybackDevice","default").toString();
    settings.endGroup();
    settings.beginGroup("User");
      MCB->Callsign = settings.value("MyCall","unknown").toString();
      MCB->GridSquare = settings.value("GridSquare","XX00xx").toString();
//      MCB->Registration = settings.value("RegistrationNum","unreg").toString();
    settings.endGroup();
    settings.beginGroup("Modem");
      // general settings to local vars here
      MCB->CWID = settings.value("CWId",false).toBool();
      MCB->DebugLog = settings.value("DebugLog",false).toBool();
      MCB->Bandwidth = settings.value("BandWidth",500).toInt();
      MCB->TxDriveLevel = settings.value("DriveLevel",50).toInt();
    settings.endGroup();
    settings.beginGroup("Network");
      // general settings to local vars here
      d->id = settings.value("NetworkConfiguration","127.0.0.1").toString();
      MCB->TCPAddress = settings.value("Address","127.0.0.1").toString();
      MCB->TCPPort = settings.value("TCPControlPort",8515).toInt();
    settings.endGroup();
}

void ARDOPModem::saveSettings()
{
    // use TNC setup menu to save settings
}

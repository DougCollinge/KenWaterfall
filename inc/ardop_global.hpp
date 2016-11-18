#ifndef ARDOP_GLOBAL_HPP
#define ARDOP_GLOBAL_HPP
/*! \file ardop_global.hpp */
/*! \defgroup ARDOP */
/*! \ingroup ARDOP */
#include <math.h>                        // for some constants like PI

#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtGui/QColor>

#define APP_VERSION "v0.8.2"
#define APP_NAME    "ARDOP TNC"

/*! \ingroup ARDOPTNC */
#include <stdint.h>
#include <complex>
using namespace std; /* Because of the library: "complex" */

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QMutex>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QVarLengthArray>
#include <QtCore/QQueue>
#include <QtCore/QWaitCondition>
#include <QtGui/QColor>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>

//--------------------------------------
// Global Defines
//--------------------------------------
#define ColorPowderBlue   QColor(179,240,255)
#define ColorDarkGray     QColor(150,150,150)
#define ColorLightGreen   QColor(200,255,200)
#define ColorLime         QColor(20,240,20)
#define ColorLightSalmon  QColor(255,200,150)
#define ColorDeepSkyBlue  QColor(0,30,100)
#define ColorTomato       QColor(255,33,00)
#define ColorGold         QColor(255,230,50)
#define ColorOrange       QColor(255,153,0)
#define ColorKhaki        QColor(220,160,0)
#define ColorSilver       QColor(100,100,140)
#define ColorYellow       Qt::yellow
#define ColorWhite        Qt::white
#define ColorSystemButton Qt::lightGray
// waveform display
#define SPECTRUM_WIDTH    256
#define SPECTRUM_HEIGHT    52
//
#define BUFSIZE           4096



//  Structure for passing TNC status
//  via synchronous queue
struct Status
{
    QString ControlName;
    QString Text;
    qint32  Value;
    QColor  BackColor;
};  // Status

// ModemControlBlock struct contains all
// TNC setup parameters (saved to ini file)
// and control parameters for operation
// This is the definition - not the actual instance
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

// RadioControlBlock struct contains all
// TNC setup parameters (saved to ini file)
// and control parameters for operation
// This is the definition - not the actual instance
struct RadioControlBlock
{
};

//--------------------------------------
// Global Vector Lists
//--------------------------------------
static QVector<QString> AllModes =
{
    "8FSK.200.25",
    "4FSK.200.50S",
    "4FSK.200.50",
    "4PSK.200.100S",
    "4PSK.200.100",
    "8PSK.200.100",
    "16FSK_500.25S",
    "16FSK.500.25",
    "4FSK.500.100S",
    "4FSK.500.100",
    "4PSK.500.100",
    "8PSK.500.100",
    "4PSK.500.167",
    "8PSK.500.167",
    "4FSK.1000.100",
    "4PSK.1000.100",
    "8PSK.1000.100",
    "4PSK.1000.167",
    "8PSK.1000.167",
    "4FSK.2000.600S",
    "4FSK.2000.600",
    "4FSK.2000.100",
    "4PSK.2000.100",
    "8PSK.2000.100",
    "4PSK.2000.167",
    "8PSK.2000.167"
};

static QVector<QString> ARQBandwidths =
{
    "200FORCED",
    "500FORCED",
    "1000FORCED",
    "2000FORCED",
    "200MAX",
    "500MAX",
    "1000MAX",
    "2000MAX"
};

static QVector<QString> HostCommands =
{
    "ABORT",           // 0
    "ARQBW",           // 1
    "ARQCALL",         // 2
    "ARQTIMEOUT",      // 3
    "BUFFER",          // 4
    "CAPTURE",         // 5
    "CAPTUREDEVICES",  // 6
    "CLOSE",           // 7
    "CMDTRACE",        // 8
    "CODEC",           // 9
    "CWID",            // 10
    "DATATOSEND",      // 11
    "DEBUGLOG",        // 12
    "DISCONNECT",      // 13
    "DISPLAY",         // 14
    "DRIVELEVEL",      // 15
    "FEDID",           // 16
    "FECMODE",         // 17
    "FECREPEATS",      // 18
    "FECSEND",         // 19
    "GRIDSQUARE",      // 20
    "INITIALIZE",       // 21
    "LEADER",           // 22
    "LISTEN",           // 23
    "MYAUX",            // 24
    "MYCALL",           // 25
    "PLAYBACK",         // 26
    "PLAYBACKDEVICES",  // 27
    "PROTOCOLMODE",     // 28
    "PURGEBUFFER",      // 29
    "RADIOANT",         // 30
    "RADIOCONTROL",     // 31
    "RADIOCTRLBAUD",    // 32
    "RADIOCTRLDTR",     // 33
    "RADIOCTRLPORT",    // 34
    "RADIOCTRLRTS",     // 35
    "RADIOFILTER",      // 36
    "RADIOFREQ",        // 37
    "RADIOIOCOMDADD",   // 38
    "RADIOISC",         // 39
    "RADIOMENU",        // 40
    "RADIOMODE",        // 41
    "RADIOMODEL",       // 42
    "RADIOPTT",         // 43
    "RADIOPTTDTR",      // 44
    "RADIOPTTRTS",      // 45
    "RDY",              // 46
    "RESTOREBUFFER",    // 47
    "SENDID",           // 48
    "SETUPMENU",        // 49
    "SQUELCH",          // 50
    "STATE",            // 51
    "TRAILER",          // 52
    "TUNERANGE",        // 53
    "TWOTONETEST",      // 54
    "VERSION"           // 55
};

static QVector<QByteArray> FrameTypes =
{
    QByteArray::fromHex("24"),    // 0:ACK
    QByteArray::fromHex("25"),    // 1:ACK
    QByteArray::fromHex("26"),    // 2:ACK
    QByteArray::fromHex("27"),    // 3:ACK
    QByteArray::fromHex("29"),    // 4:DISC
    QByteArray::fromHex("30"),    // 5:ID FRAME
    QByteArray::fromHex("31"),    // 6:Connect Request
    QByteArray::fromHex("32"),    // 7:Connect Request
    QByteArray::fromHex("33"),    // 8:Connect Request
    QByteArray::fromHex("34"),    // 9:Connect Request
    QByteArray::fromHex("40"),    // 10:OK
    QByteArray::fromHex("41"),    // 11:OK
    QByteArray::fromHex("42"),    // 12:OK
    QByteArray::fromHex("43"),    // 13:OK
    QByteArray::fromHex("44"),    // 14:OK
    QByteArray::fromHex("45"),    // 15:OK
    QByteArray::fromHex("46"),    // 16:4FSK DATA
    QByteArray::fromHex("47"),    // 17:4FSK DATA
    QByteArray::fromHex("48"),    // 18:4FSK DATA
    QByteArray::fromHex("49"),    // 19:4FSK DATA
    QByteArray::fromHex("4A"),    // 20:4FSK DATA
    QByteArray::fromHex("4B"),    // 21:4FSK DATA
    QByteArray::fromHex("4C"),    // 22:4FSK DATA
    QByteArray::fromHex("4D"),    // 23:4FSK DATA
    QByteArray::fromHex("50"),    // 24:2 CARRIER DATA FRAME
    QByteArray::fromHex("51"),    // 25:2 CARRIER DATA FRAME
    QByteArray::fromHex("52"),    // 26:2 CARRIER DATA FRAME
    QByteArray::fromHex("53"),    // 27:2 CARRIER DATA FRAME
    QByteArray::fromHex("54"),    // 28:2 CARRIER DATA FRAME
    QByteArray::fromHex("55"),    // 29:2 CARRIER DATA FRAME
    QByteArray::fromHex("56"),    // 30:2 CARRIER DATA FRAME
    QByteArray::fromHex("57"),    // 31:2 CARRIER DATA FRAME
    QByteArray::fromHex("60"),    // 32:1000 HZ DATA FRAME
    QByteArray::fromHex("61"),    // 33:1000 HZ DATA FRAME
    QByteArray::fromHex("62"),    // 34:1000 HZ DATA FRAME
    QByteArray::fromHex("63"),    // 35:1000 HZ DATA FRAME
    QByteArray::fromHex("64"),    // 36:1000 HZ DATA FRAME
    QByteArray::fromHex("65"),    // 37:1000 HZ DATA FRAME
    QByteArray::fromHex("66"),    // 38:1000 HZ DATA FRAME
    QByteArray::fromHex("67"),    // 39:1000 HZ DATA FRAME
    QByteArray::fromHex("68"),    // 40:
    QByteArray::fromHex("69"),    // 41:
    QByteArray::fromHex("70"),    // 42:2000 HZ DATA FRAME
    QByteArray::fromHex("71"),    // 43:2000 HZ DATA FRAME
    QByteArray::fromHex("72"),    // 44:2000 HZ DATA FRAME
    QByteArray::fromHex("73"),    // 45:2000 HZ DATA FRAME
    QByteArray::fromHex("74"),    // 46:2000 HZ DATA FRAME
    QByteArray::fromHex("75"),    // 47:2000 HZ DATA FRAME
    QByteArray::fromHex("76"),    // 48:2000 HZ DATA FRAME
    QByteArray::fromHex("77"),    // 49:2000 HZ DATA FRAME
    QByteArray::fromHex("78"),    // 50:
    QByteArray::fromHex("79"),    // 51:
};

static QVector<QString> SupportedRadios =
{
    "none",
    "Elecraft Radios",
    "Elecraft K2",
    "Elecraft K3",
    "Flex radios",
    "Icom Amateur Radios",
    "Icom 7000",
    "Icom 7100",
    "Icom 7200",
    "Icom 7300",
    "Icom 7410",
    "Icom 7600",
    "Icom 9100",
    "Icom 746",
    "Icom 746Pro",
    "Icom Amateur Radios (Early)",
    "Icom HF Marine Radios",
    "Kenwood Amateur",
    "Kenwood TS-2000",
    "Kenwood TS-480HX",
    "Kenwood TS-480SAT",
    "Kenwood TS-870",
    "Kenwood TS-570",
    "Kenwood TS-590S",
    "Kenwood TS-590SG",
    "Kenwood Commercial",
    "Micom 3F",
    "Ten-Tec Orion",
    "Ten-Tec Omni-7",
    "Ten-Tec Eagle",
    "Ten-Tec Jupiter",
    "Yaesu FT-100",
    "Yaesu FT-450",
    "Yaesu FT-600",
    "Yaesu FT-817",
    "Yaesu FT-840",
    "Yaesu FT-847",
    "Yaesu FT-857",
    "Yaesu FT-897",
    "Yaesu FT-920",
    "Yaesu FT-950",
    "Yaesu FT-1000",
    "Yaesu FT-2000"
};

//--------------------------------------
// Constants
//--------------------------------------
typedef const double *cdoubleptr;

// Each pass of the FFT processes 2^X samples, where X is the
// number below.
static const qint32 FFTLengthPowerOfTwo = 12;

// frame types
static const quint8 FRAME_4FSKBREAK = 35;
static const quint8 FRAME_ACK0    = 36;
static const quint8 FRAME_ACK1    = 37;
static const quint8 FRAME_ACK2    = 38;
static const quint8 FRAME_ACK3    = 39;
static const quint8 FRAME_DISC    = 41;
static const quint8 FRAME_BREAK   = 42;
static const quint8 FRAME_END     = 44;
static const quint8 FRAME_CONREJBUSY = 45;
static const quint8 FRAME_CONREJBW = 46;
static const quint8 FRAME_DISCREQ = 47;
static const quint8 FRAME_ID      = 48;
static const quint8 FRAME_CONREQ1 = 49;
static const quint8 FRAME_CONREQ2 = 50;
static const quint8 FRAME_CONREQ3 = 51;
static const quint8 FRAME_CONREQ4 = 52;
static const quint8 FRAME_CONACK200 = 57;
static const quint8 FRAME_CONACK500 = 58;
static const quint8 FRAME_CONACK1K  = 59;
static const quint8 FRAME_CONACK2K  = 60;
static const quint8 FRAME_OK0     = 64;
static const quint8 FRAME_OK1     = 65;
static const quint8 FRAME_OK2     = 66;
static const quint8 FRAME_OK3     = 67;
static const quint8 FRAME_OK4     = 68;
static const quint8 FRAME_OK5     = 69;
static const quint8 FRAME_4FSK_DATA0 = 70;
static const quint8 FRAME_4FSK_DATA1 = 71;
static const quint8 FRAME_4FSK_DATA2 = 72;
static const quint8 FRAME_4FSK_DATA3 = 73;
static const quint8 FRAME_4FSK_DATA4 = 74;
static const quint8 FRAME_4FSK_DATA5 = 75;
static const quint8 FRAME_4FSK_DATA6 = 76;
static const quint8 FRAME_4FSK_DATA7 = 77;
static const quint8 FRAME_8FSK_DATA0 = 78;
static const quint8 FRAME_CARR_DATA0 = 80;
static const quint8 FRAME_CARR_DATA1 = 81;
static const quint8 FRAME_CARR_DATA2 = 82;
static const quint8 FRAME_CARR_DATA3 = 83;
static const quint8 FRAME_CARR_DATA4 = 84;
static const quint8 FRAME_CARR_DATA5 = 85;
static const quint8 FRAME_CARR_DATA6 = 86;
static const quint8 FRAME_CARR_DATA7 = 87;
static const quint8 FRAME_16FSK500_16 = 90;
static const quint8 FRAME_16QAM_128  = 92;
static const quint8 FRAME_1K_DATA0   = 96;
static const quint8 FRAME_1K_DATA1   = 97;
static const quint8 FRAME_1K_DATA2   = 98;
static const quint8 FRAME_1K_DATA3   = 99;
static const quint8 FRAME_1K_DATA4   = 100;
static const quint8 FRAME_1K_DATA5   = 101;
static const quint8 FRAME_1K_DATA6   = 102;
static const quint8 FRAME_1K_DATA7   = 103;
static const quint8 FRAME_1K_DATA8   = 104;
static const quint8 FRAME_RESERVED69 = 105;
static const quint8 FRAME_2K_DATA0   = 112;
static const quint8 FRAME_2K_DATA1   = 113;
static const quint8 FRAME_2K_DATA2   = 114;
static const quint8 FRAME_2K_DATA3   = 115;
static const quint8 FRAME_2K_DATA4   = 116;
static const quint8 FRAME_2K_DATA5   = 117;
static const quint8 FRAME_2K_DATA6   = 118;
static const quint8 FRAME_2K_DATA7   = 119;
static const quint8 FRAME_2K_DATA8   = 120;
static const quint8 FRAME_RESERVED79 = 121;
static const quint8 FRAME_2K_DATA9   = 122;
static const quint8 FRAME_2K_DATA10  = 124;

static QString shortName;

// Compile-time calculation of powers of two
template<int N> class PowerOfTwo
{
public:
    static const qint32 Result = PowerOfTwo<N-1>::Result * 2;
};

template<> class PowerOfTwo<0>
{
public:
    static const qint32 Result = 1;
};

static QString strProductVersion = QApplication::applicationVersion();

//--------------------------------------
// Constants used in multiple locations
//--------------------------------------
// Ideally, these would match the QAudio*::bufferSize(), but that isn't
// available until some time after QAudio*::start() has been called, and we
// need this value in order to initialize the waveform display.
// We therefore just choose a sensible value.
static const qint16  PCMS16MaxValue     =  32767;
static const quint16 PCMS16MaxAmplitude =  32768; // because minimum is -32768
static const qint32  SpectrumLengthSamples    = PowerOfTwo<FFTLengthPowerOfTwo>::Result; //!< Number of audio samples used to calculate the frequency spectrum
static const qint32  SpectrumNumBands         = 10; //!< Number of bands in the frequency spectrum
static const qint64  WaveformWindowDuration   = 500 * 1000; //!< Waveform window size in microseconds
static const qreal   SpectrumLowFreq          = 200.0; //!< in Hz.    Lower bound of first band in the spectrum
static const qreal   SpectrumHighFreq         = 3500.0; //!< in Hz. Upper band of last band in the spectrum
static const qreal   SpectrumAnalyserMultiplier = 0.15; //!< Fudge factor used to calculate the spectrum bar heights
static const QString strExecutionDirectory    = QApplication::applicationDirPath();

#endif // ARDOP_GLOBAL_HPP

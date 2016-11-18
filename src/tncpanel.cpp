#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QBuffer>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QSemaphore>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsWidget>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QtCore/QMessageLogContext>

#include "vumeterwidget.hpp"
#include "tncsetup.hpp"

#include "tncpanel.hpp"

enum DispType
{
    Waterfall,
    Spectrum,
    Disabled
};

enum Mode
{
    Narrow,
    Wide
};


class TNCPanel::MainPrivate
{
public:
    // Booleans
    bool blnFormInitialized;
    bool blnSCCapturing;
    bool blnStartCodec;
    bool blnGraphicsCleared;
    bool blnSoundStreamPlay;
    bool blnLatencyCalculated;
    bool blnPullMode;
    bool blnInitialized;

    quint32  length;
    quint32  slices;
    quint32  isamplingrate;

    // Integers
    quint16 intRxVolume;
    quint16 intTxDrive;
    quint32 intRepeatCnt;
    quint32 intCaptureBufferSize;
    quint32 intPTTOnCount;
    quint32 intPTTOffCount;

    qint32 intNextCaptureOffset;
    qint32 intBMPSpectrumWidth;
    qint32 intBMPSpectrumHeight;
    qint32 intNotifySize;          //!< 2048 bytes or 1024 16 bit samples (nominally every 85.3333 ms @ 12000 sample rate) May be possible to go down to 1024 byte buffer which reduces the latency by 43 ms
    qint32 intMeasuredTRLatency;
    qint32 intAverageTRLatency;
    qint32 intLatencyCalcs;

    QVarLengthArray<qint32> intSamples;


    // Doubles
    double dblPhase;
    double dblPTTOnLevelSum;
    double dblPTTOffLevelSum;
    double maxdb;
    double range;
    double avg;

    QVarLengthArray<double> dblPhaseInc;
    QVarLengthArray<double> dblCPPhaseOffset;
    QVarLengthArray<double> dblCarFreq;

    //  Bytes
    QByteArray bytLastACKedDataFrameType;
    QByteArray bytToSend;
    QByteArray bytSymToSend;
    QByteArray bytHostIBData_CmdBuffer;

    QByteArray mywindowstate;
    QByteArray mywindowgeometry;

    // Strings
    QString strTCPIPConnectionID;
    QString strFault;
    QString strCaptureDev;
    QString strPlayDev;
    QString appver;

    // Dates
    QDateTime dttPTTApply;
    QDateTime dttPTTRelease;
    QDateTime dttNextPlay;
    QDateTime dttLastSoundCardSample;
    QDateTime dttCodecStarted;
    QDateTime dttNextGraphicsReset;

    QList<QAudioDeviceInfo> cllCaptureDevices;  //!< CaptureDevicesCollection
    QList<QAudioDeviceInfo> cllPlaybackDevices; //!< PlaybackDevicesCollection

    QAudioDeviceInfo devCaptureDevice;
    QAudioDeviceInfo devSelectedPlaybackDevice;  // Device Info Selected

    QAudioFormat   stcSCFormat;

    QSemaphore    stcPositionNotify;

    DispType m_disptype;
    Mode     m_mode;

    QBuffer       *objPlayback; // for .wav playback

    QAudioInput   *audioInput;
    QAudioOutput  *audioOutput;
    QIODevice     *audioIO;

    QTimer        *tmrPoll;
    QTimer        *tmrStartCODEC;

//    AudioIO       *audioIOObject;
};


TNCPanel::TNCPanel(QWidget *parent) : QMainWindow(parent)
{
    QT_MESSAGELOGCONTEXT
    qInstallMessageHandler(debugMessageOutput);

    // initialize the global control structures
    MCB = new ModemControlBlock;  // this is the only instance taken from globals
    RCB = new RadioControlBlock;  // this is the only instance taken from globals
    // initialize local vars
    mwd = new MainPrivate;        // create local var definitions
    mwd->blnInitialized = false;
    mwd->stcPositionNotify.acquire(mwd->intNotifySize);
    initializeLocalVars();  // must come first to init all local vars
    // create GUI
    ui = new Ui::TNCPanel;
    ui->setupUi(this);

//    mwd->vumeter = new VUMeterWidget(ui->centralWidget);
//    mwd->vumeter->move(152,6);
//    mwd->vumeter->resize(103,23);
    // fill device lists
    mwd->cllCaptureDevices  = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    mwd->cllPlaybackDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    // load stored settings from ini
    readSettings();
    initializeFromIni();

    // put a note in the log
    qInfo() << " - initializing";

    // change display to setup defaults
    QString title = tr("ARDOP Virtual TNC  v");
    title += QApplication::applicationVersion().trimmed();
    setWindowTitle(title);

    ui->retranslateUi(this);
    // finish initialization
    initializeAudio();

    createLocalObjects();
}

TNCPanel::~TNCPanel()
{
    // do an orderly shutdown to avoid memory leaks
    delete mwd->audioInput;
//    delete mwd->audioIOObject;
    delete mwd->audioIO;
    delete ui;
    delete mwd;
}

// ------------------------------------------------
//            GUI EVENT HANDLERS
// ------------------------------------------------
void TNCPanel::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TNCPanel::closeEvent(QCloseEvent *)
{
    // called before deconstructor
    try
    {
        if ( objTNCSetup )   objTNCSetup  = 0;
    }
    catch (...)
    {
        // don't do anything
    }

    mwd->mywindowgeometry = saveGeometry();
    mwd->mywindowstate    = saveState();
    writeSettings();
}

void TNCPanel::showEvent(QShowEvent *event)
{
    // after load and before display
    // or after repaint
    Q_UNUSED(event) // to avoid warnings

    QDir dir;
    QString path = strExecutionDirectory + "Logs";
    if ( !mwd->blnFormInitialized )
    {
        if ( !dir.exists(path) )
        {
            dir.mkdir(path);
        }
        path = strExecutionDirectory + "Wav";
        if ( !dir.exists(path) )
        {
            dir.mkdir(path);
        }
        strWavDirectory = path + "/";
        strWavDirectory = QDir::fromNativeSeparators(strWavDirectory);
        // Set inital window position and size...

        mwd->dttLastSoundCardSample = QDateTime::currentDateTime();
        mwd->tmrPoll->start();
        QString strFault = "";
        mwd->dttNextGraphicsReset = QDateTime::currentDateTime().addSecs(3600);

        strFault = "";
        if ( !mwd->blnInitialized )
        {
            qInfo() << "Initializing Audio";
            initializeAudio();

            qDebug() << "Audio Initialized -- connecting signals and actions";
            connect(ui->actionExit,      SIGNAL(triggered(bool)),this,SLOT(slotClose()));
            connect(ui->actionSetup,  SIGNAL(triggered(bool)),this,SLOT(slotSetupTNC(bool)));
            connect(ui->actionSetupRadio,SIGNAL(triggered(bool)),this,SLOT(slotSetupRadio(bool)));
            mwd->blnInitialized = true;
        }
    }
}

// ------------------------------------------------
//              PUBLIC SLOT HANDLERS
// ------------------------------------------------
void TNCPanel::slotCntrlChg(QString ControlName, QColor Color, QString Text, quint32 Value)
{
    Q_UNUSED(Color)

    if ( ControlName == "ToolStripLatency" )
    {
        if ( Value == 0 )
        {
            ui->lblLatency->setVisible(false);
        }
        else
        {
            ui->lblLatency->setText(Text);
            ui->lblLatency->setVisible(true);
        }
    }
    else if (ControlName == "mnuBusy" )
    {
        if ( Text == "FALSE" )
        {
            ui->ledBZY->setPixmap(QPixmap(":image/greenled.png"));
        }
        else
        {
            ui->ledBZY->setPixmap(QPixmap(":image/yelled.png"));
        }
    }
    else if ( ControlName == "lblOffset" )
    {
        ui->lblOffset->setText(Text);
    }
    else if ( ControlName == "lblRcvFrame" )
    {
        ui->lblRxFrame->setText(Text);
    }
    else if ( ControlName == "lblHost")
    {
        ui->lblHost->setText(Text);
    }
}

// ------------------------------------------------
//               PUBLIC METHODS
// ------------------------------------------------
/*
bool TNCPanel::KeyPTT(bool blnPTT)
{
    //  Returns TRUE if successful False otherwise
//    if ( blnLastPTT && !blnPTT ) objProtocol->dttStartRTMeasure = QDateTime::currentDateTime(); // start a measurement on release of PTT.
    // the following is used in T>R Latency measurements

    // If PTT goes Active clear the counts and
    // sums for PTT Latency Measurement.
    if ( blnPTT )
    {
        mwd->dttPTTApply = QDateTime::currentDateTime();
        ui->ledBZY->setPixmap(QIcon(":image/redled.png"));
    }
    else
    {
        mwd->dttPTTRelease = QDateTime::currentDateTime();
        mwd->blnLatencyCalculated = false;
        ui->ledBZY->setPixmap(QIcon(":image/greyled.png"));
    }
    if ( !RCB->RadioControl )
        objHI->SendCommandToHost("PTT " + QString("%1").arg(blnPTT).toUpper());
    else if ( objRadio )
        objRadio->PTT(blnPTT);
    if ( MCB->DebugLog ) qDebug() << "[Main.KeyPTT] " << blnPTT;
    blnLastPTT = blnPTT;
    return true;
} // KeyPTT()
*/

void TNCPanel::setRadioMenuEnabled(bool b)
{
    ui->actionSetupRadio->setEnabled(b);
}


/* To play a .wav file
bool TNCPanel::PlaySoundStream()
{
    // Plays the .wav stream with the selected Playback device
    // Returns True if no errors False otherwise...
    // Updated 0.5.0.5 July 21, 2016 to add Synclock to memwavestream
    // Todo: replace next line with RxBuffer.
//    static bufPlaybackFlags As New Microsoft.DirectX.DirectSound.BufferDescription

    bool blnExit = false;
    qint32 intTrace = 0;
    QDateTime dttStartPlay;
    Status stcStatus = NULL;


    if ( !blnCodecStarted ) return false;

//    SyncLock memWaveStreamLock
    // memWaveStream Class
    //
    // A stream is an abstraction of a sequence of bytes.
    // This sequence of bytes may come from a file, a
    // TCP/IP socket, or in this case, memory. A stream
    // is represented, aptly, by the Stream class.
    // The Stream class provides a generic view of a
    // sequence of bytes. A Stream class forms the
    // abstract class of all other streams. That is,
    // you cannot directly create an instance of the
    // Stream class. Instead, the Stream class is
    // implemented by the following classes:
    //
    //
    // BufferedStream: provides a buffering layer on
    //                 another stream to improve performance.
    // FileStream:     provides a way to read and write files.
    // MemoryStream:   provides a stream using memory
    //                 as the backing store.
    // NetworkStream:  provides a way to access data on the network.
    // CryptoStream:   provides a way to supply data
    //                 for cryptographic transformation.
    // The Qt Framework handles data streams to I/O devices (even
    // memory) by using the generic QIODevice class.
    // This class will provide functionality to data streams in a
    // generic way regardless of the actual device.

    if ( !mwd->memWaveStream )
    {
        qCritical("[Main.PlaySoundStream] memWaveStream is nothing");
        blnExit = true;
    }
    else if ( m_soundIsPlaying )
    {
        qCritical("[Main.PlaySoundStream] Sound is already playing, Call AbortWaveStream");
        AbortSoundStream();
    }
//    End SyncLock
    if ( blnExit ) return false;
    intTrace = 1;

    try
    {
        // We are using AudioIO for input and output instead of
        // platform defined devices. The MCB contains the name
        // of the current device names.
//        devSelectedPlaybackDevice.SetCooperativeLevel(Me.Handle, CooperativeLevel.Priority)
        KeyPTT(true); // Activate PTT before starting sound play
        if ( !bufPlaybackFlags.ControlVolume )
        {
            // The following flags required to allow
            // playing when not in focus and
            // to allow adjusting the volume...
            intTrace = 2;
//            bufPlaybackFlags.Flags = CType(BufferDescriptionFlags.GlobalFocus + BufferDescriptionFlags.ControlVolume, BufferDescriptionFlags)
        }
        intTrace = 3;
    }
    catch (exception ex)
    {
        qCritical("[Main.PlaySoundStream] Trace =" + QString("%1").arg(intTrace) + "  Kill PTT exception: " + ex.what());
        KeyPTT(false);
        mwd->blnSoundStreamPlay = false;
        return false;
    }

//    SyncLock memWaveStreamLock
    try
    {
        memWaveStream.Seek(0, SeekOrigin.Begin) ' reset the pointer to the origin
            intTrace = 4
            If Not IsNothing(objPlayback) Then
                objPlayback.Dispose()
                objPlayback = Nothing
            End If
            objPlayback = New SecondaryBuffer(memWaveStream, bufPlaybackFlags, devSelectedPlaybackDevice)
            intTrace = 5
            objPlayback.Volume = Math.Min(-5000 + 50 * MCB.DriveLevel, 0)  ' -5000=off, 0=full volume
            intTrace = 6
            objPlayback.Play(0, BufferPlayFlags.Default)
            intTrace = 7
            dttTestStart = Now
            dttStartPlay = Now
            intTrace = 8
            If MCB.DebugLog Then Logs.WriteDebug("[PlaySoundStream] " & strLastWavStream)
            ui->lblXmtFrame->setText(strLastWavStream);
            ui->lblXmtFrame->setStyleSheet("QLabel {background-color: ColorLightSalmon;}");
            queTNCStatus.enqueue(stcStatus);
            intTrace = 9;
            if ( MCB->DebugLog ) qDebug("[Main.PlaySoundStream] set blnSoundStreamPlay to True");
            mwd->blnSoundStreamPlay = true;
            blnExit = false;
    }
    catch (exception ex)
    {
        qCritical("[Main.PlaySoundStream] Trace =" + QString("%1").arg(intTrace) + "  Kill PTT exception: " + ex.what());
        blnExit = true;
        KeyPTT(false);
        mwd->blnSoundStreamPlay = false;
    }
    //End SyncLock
    return !blnExit;
}  // PlaySoundStream()
*/

/*
void TNCPanel::SendTestFrame(QVarLengthArray<qint32> intFilteredSamples, QString strFileStream, qint32 intRepeats)
{
    strLastWavStream = strFileStream;
    ClearTuningStats();
    ClearQualityStats();
    objMod->CreateWaveStream(intFilteredSamples);
    RxState = SearchingForLeader;
    if ( !m_soundIsPlaying )
    {
        PlaySoundStream();
        dblMaxLeaderSN = 0;
        mwd->intRepeatCnt = intRepeats;
    }
}  // SendTestFrame()
*/

/*
void TNCPanel::SendFrame(QVarLengthArray<qint32> intFilteredSamples, QString strLastFileStream, qint32 intDelayMs = 0)
{
    QDateTime dttStartWait = QDateTime::currentDateTime();
    if ( m_soundIsPlaying )
    {
        bool tim = QDateTime::currentDateTime().addMSecs(-dttStartWait)/1000 < 10;
        while ( m_soundIsPlaying && tim) // wait 10 sec max
        {
            QThread::sleep(20);
        }
    }
    mwd->dttNextPlay = QDateTime::currentDateTime().addMSecs(intDelayMs);
    objMod->CreateWaveStream(intFilteredSamples);
    if ( MCB->DebugLog ) qDebug("[Main.SendFrame] Send " + strLastFileStream + "  delayed " + QString("%1").arg(intDelayMs) + " ms");
    blnFramePending = true;
} // SendFrame()
*/

/*
void TNCPanel::SendID(bool blnEnableCWID)
{
    QString strFilename = "";
    QByteArray bytEncodedBytes;
    QByteArray bytIDSent;

    if ( MCB->GridSquare.isEmpty() )
    {
        bytEncodedBytes = objMod->Encode4FSKIDFrame(MCB->Callsign, "NO-GS", strFilename);
        bytIDSent = GetBytes(" " + MCB->Callsign + ":[NO-GS] ");
    }
    else
    {
        bytEncodedBytes = objMod->Encode4FSKIDFrame(MCB->Callsign, MCB->GridSquare, strFilename);
        bytIDSent = GetBytes(" " & MCB->Callsign + ":[" + MCB->GridSquare + "] ");
    }

    objProtocol->AddTagToDataAndSendToHost(bytIDSent, "IDF");
    mwd->intSamples = objMod->Mod4FSKData(48, bytEncodedBytes);
    if ( MCB->CWID )
    {
        mwd->intSamples.reserve(mwd->intSamples.length() + 4800);
        QVarLengthArray<qint32> intCWID;
        qint32 intPtr = mwd->intSamples.length();
        objMod->CWID("DE " + MCB->Callsign, intCWID, false);
        mwd->intSamples.reserve(mwd->intSamples.length() + intCWID.length());
        ArrayCopy(intCWID, 0, mwd->intSamples, intPtr, intCWID.length());
        strLastWavStream += " + CWID";
    }

    if ( m_soundIsPlaying )
    {
        objMod->CreateWaveStream(mwd->intSamples);
        PlaySoundStream();
    }
} //  SendID()
*/

/*
void TNCPanel::Update4FSKConstellation(QVarLengthArray<qint32> *intToneMags, qint32 *intQuality)
{
    Q_UNUSED(intToneMags)
    Q_UNUSED(intQuality)

    // Subroutine to update bmpConstellation plot for 4FSK modes...
    if ( intToneMags->size() < 40 ) return;
    try
    {
        qint32 intToneSum = 0;
        qint32 intMagMax  = 0;
        qint32 yCenter    = 0;
        qint32 xCenter    = 0;
        qint32 intRad     = 0;
        quint32 x         = 0;
        qint32 y          = 0;
        double dblRad     = 0.0;
        double dblPi4     = 0.25 * M_PI;
        double dblDistanceSum = 0.0;
        double dblPlotRotation = 0.0;

        Status stcStatus;
        QColor clrPixel;
        QImage *bmpConstellation = new QImage(QSize(89, 89),QImage::Format_ARGB32);

        // Draw the axis and paint the black background area
        yCenter = int((bmpConstellation.height() - 2) / 2);
        xCenter = int((bmpConstellation.width() - 2) / 2);
        for ( x = 0; x <= (bmpConstellation.width() - 1); ++i )
        {
            for ( y = 0; y <= (bmpConstellation.height() - 1); ++y )
            {
                if ( (y == yCenter) || (x == xCenter) )  bmpConstellation->setPixel(QPoint(x, y), ColorDeepSkyBlue);
            }
        }

        for ( i = 0; i <= (intToneMags->size() - 1); i += 4 ) // for the number of symbols represented by intToneMags
        {
            intToneSum = intToneMags(i) + intToneMags(i + 1) + intToneMags(i + 2) + intToneMags(i + 3);
            if ( (intToneMags(i) > intToneMags(i + 1)) &&
                 (intToneMags(i) > intToneMags(i + 2)) &&
                 (intToneMags(i) > intToneMags(i + 3))
               )
            {
                if ( intToneSum > 0 )
                {
                    qint32 xx = 42 - (intToneMags(i + 1) +
                                      intToneMags(i + 2) +
                                      intToneMags(i + 3)) /
                                      (intToneSum / 80);
                    intRad = qMax(5,xx);
                    if (intRad < 15 )
                        clrPixel = ColorTomato;
                    else if ( intRad < 30 )
                        clrPixel = ColorGold;
                    else
                        clrPixel = ColorLime;

                    bmpConstellation->setPixel(xCenter + intRad, yCenter + 1, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter + intRad, yCenter - 1, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter + intRad, yCenter + 2, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter + intRad, yCenter - 2, clrPixel); // don't plot on top of axis
                }
            else if ( (intToneMags(i + 1) > intToneMags(i)) &&
                      (intToneMags(i + 1) > intToneMags(i + 2)) &&
                      (intToneMags(i + 1) > intToneMags(i + 3))
                    )
            {
                if ( intToneSum > 0 )
                {
                    quint32 xx = 42 - (intToneMags(i) +
                                       intToneMags(i + 2) +
                                       intToneMags(i + 3)) /
                                      (intToneSum / 80);
                    intRad = qMax(5, xx);
                    if ( intRad < 15 )
                        clrPixel = ColorTomato;
                    else if ( intRad < 30 )
                        clrPixel = ColorGold;
                    else
                        clrPixel = ColorLime;

                    bmpConstellation->setPixel(xCenter + 1, yCenter + intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - 1, yCenter + intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter + 2, yCenter + intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - 2, yCenter + intRad, clrPixel); // don't plot on top of axis
                }
            }
            else if ( (intToneMags(i + 2) > intToneMags(i)) &&
                      (intToneMags(i + 2) > intToneMags(i + 1)) &&
                      (intToneMags(i + 2) > intToneMags(i + 3))
                    )
            {
                if ( intToneSum > 0 )
                {
                    qint32 xx = 42 - (intToneMags(i + 1) +
                                      intToneMags(i) +
                                      intToneMags(i + 3)) /
                                     (intToneSum / 80);
                    intRad = qMax(5,xx);
                    if ( intRad < 15 )
                        clrPixel = ColorTomato;
                    else if ( intRad < 30 )
                        clrPixel = ColorGold;
                    else
                        clrPixel = ColorLime;

                    bmpConstellation->setPixel(xCenter - intRad, yCenter + 1, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - intRad, yCenter - 1, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - intRad, yCenter + 2, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - intRad, yCenter - 2, clrPixel); // don't plot on top of axis
                }
            }
            else
            {
                if ( intToneSum > 0 )
                {
                    try
                    {
                        qint32 xx = 42 - (intToneMags(i + 1) +
                                          intToneMags(i + 2) +
                                          intToneMags(i)) /
                                         (intToneSum / 80);
                        intRad = qMax(5,xx);
                    }
                    catch (...)
                    {
                        intRad = 0;
                    }

                    if ( intRad < 15 )
                        clrPixel = ColorTomato;
                    else if ( intRad < 30 )
                        clrPixel = ColorGold;
                    else
                        clrPixel = ColorLime;

                    bmpConstellation->setPixel(xCenter + 1, yCenter - intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - 1, yCenter - intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter + 2, yCenter - intRad, clrPixel); // don't plot on top of axis
                    bmpConstellation->setPixel(xCenter - 2, yCenter - intRad, clrPixel); // don't plot on top of axis
                }
            }
            dblDistanceSum += (43 - intRad);
        }
        qint32 xx = int(100 - 2.4 * (dblDistanceSum / (intToneMags->size() / 4)));
        intQuality = qMax(0,xx); // factor 2.4 emperically chosen for calibration (Qual range 25 to 100)
        stcStatus.ControlName = "lblQuality";
        QString msg = "4FSK Quality: ";
        msg += QString("%1").arg(intQuality);
        stcStatus.Text = msg;
        queTNCStatus.enqueue(stcStatus);

        if ( MCB->AccumulateStats )
        {
            ++stcQualityStats.int4FSKQualityCnts;
            stcQualityStats.int4FSKQuality += intQuality;
        }
        stcStatus.ControlName = "ConstellationPlot";
        queTNCStatus.enqueue(stcStatus);
    }
    catch (...)
    {
        QString msg = "[Main.Update4FSKConstellation] Err: ";
        msg += QString("%1").arg(ex);
        qFatal << msg;
    }
}  // Update4FSKConstellation()
*/

/*
void TNCPanel::UpdatePhaseConstellation(QVarLengthArray<qint16> *intPhases, QVarLengthArray<qint16> *intMag, QString strMod, qint32 *intQuality, bool blnQAM = false)
{
    // Subroutine to update bmpConstellation plot for PSK modes...
    // Skip plotting and calulations of intPSKPhase(0) as this
    // is a reference phase (9/30/2014)
    try
        Dim intPSKPhase As Integer = CInt(strMod.Substring(0, 1))
        Dim dblPhaseError As Double
        Dim dblPhaseErrorSum As Double
        Dim intPSKIndex As Int32
        Dim intX, intY, intP As Int32
        Dim intRadInner, intRadOuter As Int32
        Dim dblRad As Double = 0
        Dim dblAvgRad As Double = 0
        Dim dblAvgRadOuter, dblAvgRadInner As Double
        Dim intMagMax As Double = 0
        Dim dblPi4 As Double = 0.25 * Math.PI
        Dim dbPhaseStep As Double = 2 * Math.PI / intPSKPhase
        Dim dblRadErrorInner, dblRadErrorOuter As Double
        Dim dblPlotRotation As Double = 0
        Dim stcStatus As Status
        Dim yCenter, xCenter As Integer

        Select Case intPSKPhase
            Case 4 : intPSKIndex = 0
            Case 8 : intPSKIndex = 1
        End Select

        bmpConstellation = New Bitmap(89, 89)
        ' Draw the axis and paint the black background area
        yCenter = (bmpConstellation.Height - 1) \ 2
        xCenter = (bmpConstellation.Width - 1) \ 2
        For x As Integer = 0 To bmpConstellation.Width - 1
            For y As Integer = 0 To bmpConstellation.Height - 1
                If y = yCenter Or x = xCenter Then
                    bmpConstellation.SetPixel(x, y, Color.Tomato)
                End If
            Next y
        Next x
        If blnQAM Then
            intPSKPhase = 8
            intPSKIndex = 1
            dbPhaseStep = 2 * Math.PI / intPSKPhase
            For j As Integer = 1 To intMag.Length - 1  ' skip the magnitude of the reference in calculation
                intMagMax = Math.Max(intMagMax, intMag(j)) ' find the max magnitude to auto scale
            Next j
            For k As Integer = 1 To intMag.Length - 1
                If intMag(k) < 0.75 * intMagMax Then
                    dblAvgRadInner += intMag(k) : intRadInner += 1
                Else
                    dblAvgRadOuter += intMag(k) : intRadOuter += 1
                End If
            Next k
            dblAvgRadInner = dblAvgRadInner / intRadInner
            dblAvgRadOuter = dblAvgRadOuter / intRadOuter
        Else
            For j As Integer = 1 To intMag.Length - 1  ' skip the magnitude of the reference in calculation
                intMagMax = Math.Max(intMagMax, intMag(j)) ' find the max magnitude to auto scale
                dblAvgRad += intMag(j)
            Next j
        End If

        dblAvgRad = dblAvgRad / (intMag.Length - 1) ' the average radius
        ' For i As Integer = 0 To intPhases.Length - 1
        For i As Integer = 1 To intPhases.Length - 1 ' Don't plot the first phase (reference)
            dblRad = 40 * intMag(i) / intMagMax ' scale the radius dblRad based on intMagMax
            intX = CInt(xCenter + dblRad * Math.Cos(dblPlotRotation + intPhases(i) / 1000))
            intY = CInt(yCenter + dblRad * Math.Sin(dblPlotRotation + intPhases(i) / 1000))
            intP = CInt(Math.Round(0.001 * intPhases(i) / dbPhaseStep))
            ' compute the Phase and Raduius errors
            If intMag(i) > (dblAvgRadInner + dblAvgRadOuter) / 2 Then
                dblRadErrorOuter += Abs(dblAvgRadOuter - intMag(i))
            Else
                dblRadErrorInner += Abs(dblAvgRadInner - intMag(i))
            End If
            dblPhaseError = Abs(((0.001 * intPhases(i)) - intP * dbPhaseStep)) ' always positive and < .5 *  dblPhaseStep
            dblPhaseErrorSum += dblPhaseError
            If intX <> xCenter And intY <> yCenter Then bmpConstellation.SetPixel(intX, intY, Color.Yellow) ' don't plot on top of axis
        Next i
        If blnQAM Then
            ' include Radius error for QAM ...Lifted from WINMOR....may need work
            intQuality = CInt(Math.Max(0, (1 - (dblRadErrorInner / (intRadInner * dblAvgRadInner) + dblRadErrorOuter / (intRadOuter * dblAvgRadOuter))) * (100 - 200 * (dblPhaseErrorSum / (intPhases.Length - 1)) / dbPhaseStep)))
            If MCB->AccumulateStats Then
                stcQualityStats.intQAMQualityCnts += 1
                stcQualityStats.intQAMQuality += intQuality
                stcQualityStats.intQAMSymbolsDecoded += intPhases.Length
            End If
        Else
            ' This gives good quality with probable seccessful decoding threshold around quality value of 60 to 70
            intQuality = Math.Max(0, CInt((100 - 200 * (dblPhaseErrorSum / (intPhases.Length - 1)) / dbPhaseStep))) ' ignore radius error for (PSK) but include for QAM
            If MCB->AccumulateStats Then
                stcQualityStats.intPSKQualityCnts(intPSKIndex) += 1
                stcQualityStats.intPSKQuality(intPSKIndex) += intQuality
                stcQualityStats.intPSKSymbolsDecoded += intPhases.Length
            End If
        End If

        stcStatus.ControlName = "lblQuality"
        stcStatus.Text = strMod & " Quality: " & intQuality.ToString
        queTNCStatus.Enqueue(stcStatus)
        stcStatus.ControlName = "ConstellationPlot"
        queTNCStatus.Enqueue(stcStatus)
    Catch ex As Exception
        Logs.Exception("[Main.UpdatePhaseConstellation] Err: " & ex.ToString)
    End Try
}  // UpdatePhaseConstellation()
*/



// ------------------------------------------------
//              END PUBLIC FUNCTIONS
// ------------------------------------------------

// ------------------------------------------------
//                 PRIVATE SLOTS
// ------------------------------------------------
void TNCPanel::slotReadMore()
{
    // qDebug() << "processing audio"
    // update rxBuffer with data from the producer
    quint32 i = 0;
    quint32 bl = 0;
    quint32 ds = 0;
    quint32 st = 0;
    qint64 len = 0;
    qint64 l   = 0;
    QByteArray data;
    Q_UNUSED(l)

    if ( !mwd->audioInput )  return;
    len = mwd->audioInput->bytesReady();
    if ( len > BUFSIZE )    len = BUFSIZE;
    // we must pass the reference to the local buffer not create a new one
    l = mwd->audioIO->read(RxBuffer->data(), len);
//    qDebug() << "Data:" << QString("%1").arg(l);
//    if ( l > 0 ) mwd->audioIOObject->writeData(RxBuffer->constData(), l);
//    if ( !mwd->audioIOObject )  return;
    data = mwd->audioIO->readAll();
    // get buffer length
    bl = RxBuffer->length();
    // get data size
    ds = data.size();
    // get starting slot
    st = bl - (bl - RxBuffer->NumUsedBytes);
    // if start bucket equal buffer length start over at zero
    if ( st == bl ) st = 0;
    // for start bucket to data size dump data into buffer
    for ( i = st; i < ds; ++i )
    {
        RxBuffer->mutex.lock();
        if ( (quint32)RxBuffer->NumUsedBytes == BUFSIZE )
            RxBuffer->bufferNotFull.wakeAll();
        else
            RxBuffer->bufferNotFull.wait(&RxBuffer->mutex,100);

        RxBuffer->buffer[st + i] = reinterpret_cast<const char*>(data.at(i));
        ++RxBuffer->NumUsedBytes;
        RxBuffer->mutex.unlock();
    }
    if ( ds > 0 ) mwd->audioIO->write(RxBuffer->constData(), ds);
    if ( !(quint32(RxBuffer->NumUsedBytes) == RxBuffer->getBufSize()) )
    {
//        mwd->wfwidget->updateWaterfall(RxBuffer);
        RxBuffer->bufferNotFull.wait(&RxBuffer->mutex,10);
        RxBuffer->bufferNotEmpty.wakeAll();
    }
}

void TNCPanel::slotUpdateVU()
{
    quint16 v = 0;
//    v = mwd->audioIOObject->level() * 100;
    qDebug() << "Lvl: " << QString("%1").arg(v);
    //    ui->lblLevel->setText(QString("%1").arg(v));
    if ( v > 100 ) v = 100;
    ui->progressBar->setValue(v);
    ui->progressBar->update();
}

void TNCPanel::slotClose()
{
    // capture panel geometry, state, and MCB/RCB with settings
    mwd->mywindowstate = saveState();
    mwd->mywindowgeometry = saveGeometry();
    // stop the codecs and make a graceful exit

    // save all settings to .ini
    writeSettings();
    close();
}

void TNCPanel::slotAudioErr(QString heading,QString details) { qCritical() << heading << details;  }


void TNCPanel::slotSetupTNC(bool b)
{
    Q_UNUSED(b)
    TncSetup dlgTNCSetup;
    dlgTNCSetup.exec();
}

/*
void TNCPanel::slotSetupRadio(bool b)
{
    Q_UNUSED(b)
    RadioSetup dlgRadioSetup;
    dlgRadioSetup.exec();
}
*/

void TNCPanel::slotDispTypeChgd()
{
    if ( mwd->m_disptype == Waterfall )
    {
        // display spectrum waterfall
    }
    else if ( mwd->m_disptype == Spectrum )
    {
        // display audio spectrum graph
    }
    else
    {
        // no display
//        ui->wfwidget->setVisible(false);
    }
}


// ----------------------------------------------------
//           FRIENDLY LOCAL FUNCTIONS
// ----------------------------------------------------
/*
bool TNCPanel::startCodec()
{
    // This delay is necessary for reliable startup
    // especially following stopCodec()
    QThread::sleep(100);
    qDebug() << "Waking after sleep(100)";
    // no need for thread locking here because this is the only
    // thread that executes this code.
    mwd->dttLastSoundCardSample = QDateTime::currentDateTime();
    bool blnSpectrumSave    = MCB->DisplaySpectrum;
    bool blnWaterfallSave   = MCB->DisplayWaterfall;
    QDateTime dttStartWait  = mwd->dttLastSoundCardSample;
    MCB->DisplayWaterfall    = false;
    MCB->DisplaySpectrum     = false;

    blnCodecStarted = true;
    objProtocol->ARDOPState = DISC;
    // insurance for PTT off
    MCB->PTT = false;
    if ( objRadio ) objRadio->PTT(false);
    //
    MCB->DisplayWaterfall = blnWaterfallSave;
    MCB->DisplaySpectrum = blnSpectrumSave;
    mwd->dttCodecStarted = QDateTime::currentDateTime();
    QBrush brush;
    brush.setColor(ColorLightGreen);
    brush.setStyle(Qt::SolidPattern);
    ui->lblICF->setText(tr("CODEC Start OK"));
    mwd->blnGraphicsCleared = false;
    qInfo() << tr(" - Start Codec succeeded ");
    //inititializeNotifications();
    // start the input device
    mwd->blnStartCodec = true;
    return blnCodecStarted;
} // startCodec()
*/

/*
bool TNCPanel::stopCodec()
{
    // Function to stop the Codec (sound card capture)
    bool blnStopCodec = false;
    Q_UNUSED(blnStopCodec)  // to eliminate warnings in release compile

    qInfo() << tr("Stop thrNotify with blnSCCapturing = False");
    mwd->blnSCCapturing = false;
    // this should end the wait thread if it is still running
    this->thread()->sleep(200);
    if ( this->thread()->isRunning() )
    {
        qWarning() << tr("Extending wait 100 ms for stopCodec()");
        this->thread()->sleep(100);
    }

    // Stop the buffer
//    if ( engine )
//    {
//         engine->stopCapture();
//         engine = 0;
//    }
    if ( !mwd->devCaptureDevice.isNull() )          mwd->devCaptureDevice = QAudioDeviceInfo::defaultInputDevice();
    if ( !mwd->devSelectedPlaybackDevice.isNull() ) mwd->devSelectedPlaybackDevice = QAudioDeviceInfo::defaultOutputDevice();
    qInfo() << tr("Stopping Codec ");
    QBrush brush;
    brush.setColor(ColorDarkGray);
    brush.setStyle(Qt::SolidPattern);
    ui->lblICF->setText(tr("CODEC Stopped"));
//    objProtocol.SetARDOPProtocolState(OFFLINE);
//    tmrStartCODEC->stop();
    return true;
}  // StopCodec
*/

/*
void TNCPanel::clearSpectrum()
{
    // Subroutine to initialize the Spectrum/Waterfall display to black.
//    if ( mwd->bmpSpectrum ) mwd->bmpSpectrum = 0;
//    mwd->graFrequency = ui->lblWaterfall;
//    mwd->graFrequency.setAutoFillBackground(QColor::black());
//    mwd->intBMPSpectrumWidth = 256;
//    mwd->intBMPSpectrumHeight = 62;
//    mwd->bmpSpectrum = new QBitmap(intBMPSpectrumWidth, intBMPSpectrumHeight);
//    ui->wfdisplay
}  // clearSpectrum()
*/


void TNCPanel::initializeFromIni()
{
    qInfo() << "Restoring settings read from .ini";

    restoreGeometry(mwd->mywindowgeometry);
    restoreState(mwd->mywindowstate);

    //  ui->spectrWidget->displayWaterfall(false);
    if ( ui->optWF->isChecked() )      mwd->m_disptype = Waterfall;
    else if (ui->optSP->isChecked() )  mwd->m_disptype = Spectrum;
    else                               mwd->m_disptype = Disabled;

    if ( mwd->m_disptype == Waterfall &&
         mwd->m_mode == Narrow
       )
    {
        //      ui->spectrWidget->setMarkers(725,1475,1850);
//        mwd->wfwidget->setVisible(true);
//        ui->waterfallWidget->setMarkers(725,1475,1850);
        //      ui->markerLabelSpectrum->setMarkers(725,1475,1850);
        //      ui->markerLabelWF->setMarkers(725,1475,1850);

    }
    else if ( mwd->m_disptype == Waterfall &&
              mwd->m_mode == Wide
            )
    {
        //      ui->spectrWidget->setMarkers(1200,1500,2300);
//        ui->waterfallWidget->setMarkers(1200,1500,2300);
        //      ui->markerLabelSpectrum->setMarkers(1200,1500,2300);
        //      ui->markerLabelWF->setMarkers(1200,1500,2300);
    }
    else if ( mwd->m_disptype == Spectrum )
    {
//        mwd->wfwidget->setVisible(false);
//        ui->spectrumWidget->setVisible(true);
    }

//    if ( RCB->RadioControl )
//    {
//        if ( MCB->DebugLog )
//            qInfo() << tr("TNC Panel checking for Radio Control");
//        ui->actionSetupRadio->setEnabled(true);
//        if ( objRadio )
//        {
//            if ( MCB->DebugLog )
//                qInfo() << tr("TNC Panel creating new radio control");
//            objRadio = new RadioSetup(this);
//            objRadio->getRadioSettings();
//            objRadio->InitRadioPorts();
//        }
//    }
//    else
//    {
//        ui->actionSetupRadio->setEnabled(false);
//    }
}

void TNCPanel::initializeAudio()
{
    quint16 x = 0;
    qInfo() << "Initializing Audio Devices";
    bool blnGoodFmt    = false;
    mwd->blnStartCodec = false;
    // instantiate the central buffer objects
    // this is the only place these objects are instantiated
    RxBuffer = new BufferObject;
    TxBuffer = new BufferObject;
    RxBuffer->setBufSize(4096);
    TxBuffer->setBufSize(4096);

    qDebug() << "Set up the audio engine with default format and test";

    mwd->stcSCFormat.setSampleType(QAudioFormat::SignedInt);
    mwd->stcSCFormat.setByteOrder(QAudioFormat::LittleEndian);
    mwd->stcSCFormat.setChannelCount(1);
    mwd->stcSCFormat.setSampleSize(16);
    mwd->stcSCFormat.setSampleRate(8000);
    mwd->stcSCFormat.setCodec("audio/pcm");
    //                                                          //
    qDebug() << "Now start with the default system device and test the format";
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    blnGoodFmt = info.isFormatSupported(mwd->stcSCFormat);
    if ( !blnGoodFmt )
    {
        qDebug() << "The format given is not supported by default device -- trying nearest";
        mwd->stcSCFormat = info.nearestFormat(mwd->stcSCFormat);
    }

    qInfo() << "If device exists then delete it first and start over";
//    if ( mwd->blnFormInitialized ) delete mwd->audioIOObject;
    qInfo() << "Setting audio IO device buffers to local object";
//    mwd->audioIOObject  = new AudioIO;
//    mwd->audioIOObject->setRxBuffer(RxBuffer);
//    mwd->audioIOObject->settxBuffer(TxBuffer);


    qInfo() << "Setting audio IO device to nearest format. "
            << "Format must be set before any other IO function"
            << "is called (like start or writeData() )";
//    mwd->audioIOObject->setAudioFormat(mwd->stcSCFormat);

    qDebug() << "Test to see if settings has a device specified";
    if ( mwd->strCaptureDev == "default" )
    {
        qDebug() << "None saved so give the default device a name - " << info.deviceName();
        mwd->strCaptureDev = info.deviceName();
    }
    qInfo() << "Input Dev:" << mwd->strCaptureDev;

    for ( x = 0; x < (mwd->cllCaptureDevices.count() - 1); ++x )
    {
        if ( mwd->strCaptureDev == mwd->cllCaptureDevices.at(x).deviceName() )
        {
            qDebug() << "Set the capture device info into local vars";
            mwd->strCaptureDev = mwd->cllCaptureDevices[x].deviceName();
            mwd->devCaptureDevice = mwd->cllCaptureDevices[x];
            break;
        }
    }
    qDebug() << "Dev set to:" << mwd->devCaptureDevice.deviceName();
    if ( MCB->CaptureDevice == "" ) MCB->CaptureDevice = mwd->devCaptureDevice.deviceName();
    // when the update signal (from auidoIO::writeData() is
    // received from the IO Device, update the VU Meter
    // using audioIO::level()
    createAudioInput();
}

void TNCPanel::createAudioInput()
{
    // The format and capture device must have been set
    // before calling this function since both are needed
    // to create the input object
    qInfo() << "Creating Audio Input Device with "
            << mwd->devCaptureDevice.deviceName();

    mwd->audioInput = new QAudioInput(mwd->devCaptureDevice, mwd->stcSCFormat, this);

    qInfo() << "Starting Audio IO Object ";
//    mwd->audioIOObject->start();
    qInfo() << "Starting Audio Input ";
//    mwd->audioInput->start(mwd->audioIOObject);
    qInfo() << "Setting Audio Input default volume to settings value ";
    mwd->audioInput->setVolume(mwd->intRxVolume);
    mwd->audioInput->stop();
    qInfo() << "Get the pointer to the IO Device";
    mwd->audioIO = mwd->audioInput->start();

    if ( !mwd->audioIO->isOpen() )  qDebug() << "input not open ...";
    qInfo() << "Input open ... connecting signals";
    ui->progressBar->setFormat(QString(""));
//    connect(mwd->audioIOObject, SIGNAL(update()),    this,SLOT(slotUpdateVU()));
//    connect(mwd->audioIOObject, SIGNAL(readyRead()), this,SLOT(slotReadMore()));
    // TODO: set up a QThread run()
    //       process to fill the rx buffer and signal
    //       it is full
    // TODO: set up a QThread separate from above to start()
    //       by being connected to the signal from the above
    //       thread and paint the WF display stop() and return
    //       in an orderly asynchronous fashion
    qInfo() <<"Audio Input Initialized -- returning";
}

/*
void TNCPanel::realFFT(double *iBuffer)
{
    mwd->fftFunc.realFFT(iBuffer);
//    ui->spectrWidget->showFFT(fftFunc.out);
//    ui->waterfallWidget->showFFT(fftFunc.out);
}
*/

/*
void clearTuningStats()
{
    // Clear all Tuning Stats
    stcTuningStats.intLeaderDetects             = 0;
    stcTuningStats.intLeaderSyncs               = 0;
    stcTuningStats.intFrameSyncs                = 0;
    stcTuningStats.intAccumFSKTracking          = 0;
    stcTuningStats.intFSKSymbolCnt              = 0;
    stcTuningStats.intAccumPSKTracking          = 0;
    stcTuningStats.intPSKSymbolCnt              = 0;
    stcTuningStats.intGoodFSKFrameTypes         = 0;
    stcTuningStats.intFailedFSKFrameTypes       = 0;
    stcTuningStats.intGoodFSKFrameDataDecodes   = 0;
    stcTuningStats.intFailedFSKFrameDataDecodes = 0;
    stcTuningStats.intGoodPSKFrameDataDecodes   = 0;
    stcTuningStats.intGoodPSKSummationDecodes   = 0;
    stcTuningStats.intGoodFSKSummationDecodes   = 0;
    stcTuningStats.intFailedPSKFrameDataDecodes = 0;
    stcTuningStats.intAvgFSKQuality             = 0;
    stcTuningStats.intAvgPSKQuality             = 0;
    stcTuningStats.intPSKTrackAttempts          = 0;
    stcTuningStats.intDecodeDistanceCount       = 0;
    stcTuningStats.intShiftDNs                  = 0;
    stcTuningStats.intShiftUPs                  = 0;
    stcTuningStats.dblFSKTuningSNAvg            = 0.0;
    stcTuningStats.dblLeaderSNAvg               = 0.0;
    stcTuningStats.dblAvgPSKRefErr              = 0.0;
    stcTuningStats.dblAvgDecodeDistance         = 0.0;
} // clearTuningStats()

void clearQualityStats()
{
    // Sub to Clear the Quality Stats
    stcQualityStats.int4FSKQuality               = 0;
    stcQualityStats.int4FSKQualityCnts           = 0;
    stcQualityStats.int8FSKQuality               = 0;
    stcQualityStats.int8FSKQualityCnts           = 0;
    stcQualityStats.int16FSKQuality              = 0;
    stcQualityStats.int16FSKQualityCnts          = 0;
    stcQualityStats.intPSKQuality.resize(1);     // Quality for 4PSK, 8PSK  modulation modes
    stcQualityStats.intPSKQualityCnts.resize(1); // Counts for 4PSK, 8PSK modulation modes
                                                 // need to get total quantity of PSK modes
    stcQualityStats.intFSKSymbolsDecoded = 0;
    stcQualityStats.intPSKSymbolsDecoded = 0;
}   //clearQualityStats()

void initializeConnection()
{
    // Sub to Initialize before a new Connection
    stcConnection.blnLastPSNPassed     = false; // the last PSN passed True for Odd, False for even.
    stcConnection.blnInitiatedConnection = false;  // flag to indicate if this station initiated the connection
    stcConnection.bytSessionID         = 0xFF;  // Session ID
    stcConnection.intOBBytesToConfirm  = 0;     // remaining bytes to confirm
    stcConnection.intBytesConfirmed    = 0;     // Outbound bytes confirmed by ACK and squenced
    stcConnection.intReceivedLeaderLen = 0;     // Zero out received leader length (the length of the leader as received by the local station
    stcConnection.intReportedLeaderLen = 0;     // Zero out the Reported leader length the length reported to the remote station
    stcConnection.intTotalSymbols      = 0;     // To compute the sample rate error
    stcConnection.intSessionBW         = 0;     // ExtractARQBandwidth()
    stcConnection.intCalcLeader        = MCB->LeaderLength;
    stcConnection.dblAvgPECreepPerCarrier = 0;  // computed phase error creep
    stcConnection.dttLastIDSent        = QDateTime::currentDateTime(); // date/time of last ID
    stcConnection.strRemoteCallsign    = "",     // remote station call sign
    stcConnection.strLocalCallsign     = "";    // this stations call sign

    clearQualityStats();
    clearTuningStats();
}  // InitializeConnection

void UpdateFSKFrameDecodeStats(bool blnDecodeSuccess)
{
    // Subroutine to Update FSK Frame Decoding Stats
    if ( !MCB->AccumulateStats ) return;
    if ( blnDecodeSuccess )
        ++stcTuningStats.intGoodFSKFrameTypes;
    else
        ++stcTuningStats.intFailedFSKFrameTypes;

}  //UpdateFSKFrameDecodeStats()

*/

void TNCPanel::initializeLocalVars()
{
    MCB->BusyDet              = 0;
    MCB->RxAudioSampleRate    = 12000;
    MCB->RxAudioSampleSize    = 16;
    MCB->RxDataFrameSize      = 2;
    MCB->RxSquelchLevel       = 5;

    m_soundIsPlaying = false;
    blnRadioMenuEnabled = true;
    blnCodecStarted = false;
    blnInTestMode   = false;
    blnClosing      = false;
    blnFramePending = false;
    blnLastPTT      = false;
    blnFramePending = false;

    mwd->blnFormInitialized = false;
    mwd->blnSCCapturing       = false;
    mwd->blnGraphicsCleared   = false;
    mwd->blnSoundStreamPlay   = false;
    mwd->blnLatencyCalculated = true;

    mwd->intRxVolume     = 0;
    mwd->intTxDrive      = 0;
    mwd->intRepeatCnt    = 0;
    mwd->intNotifySize   = 0;
    mwd->intPTTOnCount   = 0;
    mwd->intPTTOffCount  = 0;
    mwd->intLatencyCalcs = 0;
    mwd->intCaptureBufferSize = 0;
    mwd->intNextCaptureOffset = 0;
    mwd->intMeasuredTRLatency = 0;
    mwd->intAverageTRLatency  = 0;
    mwd->intBMPSpectrumWidth  = 256;
    mwd->intBMPSpectrumHeight = 62;
    // 2048 bytes or 1024 16 bit samples
    // (nominally every 85.3333 ms @ 12000 sample rate)
    mwd->intNotifySize        = 2048;

    mwd->maxdb    = 0.0;
    mwd->range    = 0.0;
    mwd->avg      = 0.0;
    mwd->dblPhase = 0.0;
    mwd->dblPTTOnLevelSum  = 0.0;
    mwd->dblPTTOffLevelSum = 0.0;

    mwd->intSamples.clear();
    mwd->bytHostIBData_CmdBuffer.clear();
    mwd->cllCaptureDevices.clear();
    mwd->cllPlaybackDevices.clear();
    mwd->dblPhaseInc.clear();
    mwd->dblCPPhaseOffset.clear();
    mwd->dblCarFreq.clear();

    mwd->bytLastACKedDataFrameType.clear();
    mwd->bytToSend.clear();
    mwd->bytSymToSend.clear();
    mwd->bytHostIBData_CmdBuffer.clear();

    mwd->mywindowstate.clear();
    mwd->mywindowgeometry.clear();

    // Strings
    mwd->strTCPIPConnectionID = "";
    mwd->strFault             = "";
    mwd->strCaptureDev        = "";
    mwd->strPlayDev           = "";
    mwd->appver               = "";
}

void TNCPanel::createLocalObjects()
{
//    objRadioSetup = new RadioSetup(this);
//    objRadio      = new Radio(this);
//    objBusy       = new BusyDetector(this);
}

void TNCPanel::readSettings()
{
    QDir dir;
    QSettings settings(dir.canonicalPath() + "/ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("SOUND");
     MCB->CaptureDevice  = settings.value("CaptureDevice",  "default").toString();
     MCB->PlaybackDevice = settings.value("PlaybackDevice", "default").toString();
     MCB->RxVolume       = settings.value("Volume",             50).toInt();
     MCB->TxDriveLevel   = settings.value("DriveLevel",         80).toInt();
     MCB->RxSquelchLevel = settings.value("Squelch",            20).toInt();
     MCB->Stereo         = settings.value("Stereo",             false).toBool();
     MCB->CaptureClock   = settings.value("CaptureClock",       0).toInt();
     MCB->PlaybackClock  = settings.value("PlaybackClock",      0).toInt();
    settings.endGroup();

    settings.beginGroup("NETWORK");
      MCB->TCPPort        = settings.value("TCPIPPort",         8515).toInt();
      MCB->TCPAddress     = settings.value("TCPIPAddress",      "127.0.0.1").toString();
      MCB->HostTCPIP      = settings.value("HostTCPIP",         true).toBool();
    settings.endGroup();

    settings.beginGroup("STATION");
      MCB->Callsign        = settings.value("Callsign",         "unknown").toString();
      MCB->Password        = settings.value("LoginPassword",    "").toBool();
      MCB->CWID            = settings.value("EnableCWID",       false).toBool();
      MCB->SecureHostLogin = settings.value("SecureHostLogin",  false).toBool();
    settings.endGroup();

    settings.beginGroup("TNC");
     MCB->SerBaud        = settings.value("SerialBaud",         19200).toInt();
     MCB->SerCOMPort     = settings.value("SerialCOMPort",      "COM3").toString();
     MCB->HostPairing    = settings.value("HostPairing",        "").toString();
     MCB->HostSerial     = settings.value("HostSerial",         false).toBool();
     MCB->HostBlueTooth  = settings.value("HostBlueTooth",      false).toBool();
     MCB->StartMinimized = settings.value("StartMinimized",     false).toBool();
     MCB->DebugLog       = settings.value("DebugLog",           false).toBool();
     MCB->CommandTrace   = settings.value("CommandTrace",       false).toBool();
     MCB->LeaderLength   = settings.value("LeaderLenth",        0).toInt();
     MCB->TrailerLength  = settings.value("TrailerLength",      0).toInt();
     MCB->AccumulateStats = settings.value("AccumStats",        false).toBool();
     MCB->DisplayWaterfall = settings.value("DispWaterfall",    true).toBool();
     MCB->DisplaySpectrum  = settings.value("DispSpectrum",     false).toBool();
     MCB->TuningRange    = settings.value("TuningRange",        100).toInt();
     RCB->RadioControl   = settings.value("Enbl Radio Cont",    false).toBool();
     MCB->FECRepeats     = settings.value("FECRepeats",         1).toInt();
     MCB->FECMode        = settings.value("FECMode",            "").toString();
     MCB->FECId          = settings.value("FECId",              false).toBool();
     MCB->ARQConReqRepeats = settings.value("ARQConRepReq",     5).toInt();
     MCB->ProtocolMode   = settings.value("ProtocolMode",       "").toString();
     MCB->ARQTimeout     = settings.value("ARQTimeout",         100).toInt();
     MCB->SlowCPU        = settings.value("SlowCPU",            false).toBool();
     mwd->appver         = settings.value("Version", "").toString();
    settings.endGroup();


    settings.beginGroup("RADIO");
     RCB->Filter       = settings.value("ControlFilter", 0).toInt();
     RCB->Mode         = settings.value("Mode", "").toString();
     RCB->IcomAdd      = settings.value("IcomAddress", "00").toString();
     RCB->RigModel     = settings.value("Model", "").toString();
     RCB->InternalTuner = settings.value("Tuner", false).toBool();
     RCB->InternalSoundCard = settings.value("UseIntSoundCard", false).toBool();
     RCB->PTTDTR       = settings.value("KeyPTTDTR", false).toBool();
     RCB->PTTRTS       = settings.value("KeyPTTRTS", true).toBool();
     RCB->PTTPort      = settings.value("PTTCtrlPort", "COM1").toString();
     RCB->CtrlPort     = settings.value("ControlPort", "").toString();
     RCB->CtrlPortBaud = settings.value("ControlBaud", 9600).toInt();
     RCB->CtrlPortRTS  = settings.value("ControlRTS", false).toBool();
     RCB->Ant          = settings.value("Ant", 0).toInt();
     RCB->RadioControl = settings.value("EnableRadioCtrl", false).toBool();
    settings.endGroup();
}

void TNCPanel::writeSettings()
{
    QDir dir;
    QSettings settings(dir.canonicalPath() + "/ardop_tnc.ini",QSettings::IniFormat);

    settings.beginGroup("SOUND");
     settings.setValue("CaptureDevice",  MCB->CaptureDevice);
     settings.setValue("PlaybackDevice", MCB->PlaybackDevice);
     settings.setValue("DriveLevel",     MCB->TxDriveLevel);
     settings.setValue("Squelch",        MCB->RxSquelchLevel);
     settings.setValue("Stereo",         MCB->Stereo);
     settings.setValue("CaptureClock",   MCB->CaptureClock);
     settings.setValue("PlaybackClock",  MCB->PlaybackClock);
    settings.endGroup();

    settings.beginGroup("NETWORK");
      settings.setValue("TCPIPPort",     MCB->TCPPort);
      settings.setValue("TCPIPAddress",  MCB->TCPAddress);
      settings.setValue("HostTCPIP",     MCB->HostTCPIP);
    settings.endGroup();

    settings.beginGroup("STATION");
      settings.setValue("Callsign",      MCB->Callsign);
      settings.setValue("LoginPassword", MCB->Password);
      settings.setValue("EnableCWID",    MCB->CWID);
      settings.setValue("SecureHostLogin", MCB->SecureHostLogin);
    settings.endGroup();

    settings.beginGroup("TNC");
     settings.setValue("SerialBaud",     MCB->SerBaud);
     settings.setValue("SerialCOMPort",  MCB->SerCOMPort);
     settings.setValue("HostPairing",    MCB->HostPairing);
     settings.setValue("HostSerial",     MCB->HostSerial);
     settings.setValue("HostBlueTooth",  MCB->HostBlueTooth);
     settings.setValue("StartMinimized", MCB->StartMinimized);
     settings.setValue("DebugLog",       MCB->DebugLog);
     settings.setValue("CommandTrace",   MCB->CommandTrace);
     settings.setValue("LeaderLenth",    MCB->LeaderLength);
     settings.setValue("TrailerLength",  MCB->TrailerLength);
     settings.setValue("AccumStats",     MCB->AccumulateStats);
     settings.setValue("DispWaterfall",  MCB->DisplayWaterfall);
     settings.setValue("DispSpectrum",   MCB->DisplaySpectrum);
     settings.setValue("TuningRange",    MCB->TuningRange);
     settings.setValue("EnblRadioCont",  RCB->RadioControl);
     settings.setValue("FECRepeats",     MCB->FECRepeats);
     settings.setValue("FECMode",        MCB->FECMode);
     settings.setValue("FECId",          MCB->FECId);
     settings.setValue("ARQConRepReq",   MCB->ARQConReqRepeats);
     settings.setValue("ProtocolMode",   MCB->ProtocolMode);
     settings.setValue("ARQTimeout",     MCB->ARQTimeout);
     settings.setValue("SlowCPU",        MCB->SlowCPU);
     settings.setValue("Version",        APP_VERSION);
    settings.endGroup();

    settings.beginGroup("RADIO");
     settings.setValue("ControlFilter", RCB->Filter);
     settings.setValue("ControlPort",   RCB->CtrlPort);
     settings.setValue("Mode",          RCB->Mode);
     settings.setValue("IcomAddress",   RCB->IcomAdd);
     settings.setValue("Model",         RCB->RigModel);
     settings.setValue("Tuner",         RCB->InternalTuner);
     settings.setValue("UseIntSoundCard", RCB->InternalSoundCard);
     settings.setValue("KeyPTTDTR",     RCB->PTTDTR);
     settings.setValue("KeyPTTRTS",     RCB->PTTRTS);
     settings.setValue("PTTCtrlPort",   RCB->PTTPort);
     settings.setValue("ControlBaud",   RCB->CtrlPortBaud);
     settings.setValue("ControlRTS",    RCB->CtrlPortRTS);
     settings.setValue("Ant",           RCB->Ant);
     settings.setValue("EnableRadioCtrl", RCB->RadioControl);
    settings.endGroup();

    settings.sync();
}


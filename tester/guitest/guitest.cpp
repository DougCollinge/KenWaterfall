#include <QtCore/QDebug>
#include <QtCore/QSettings>
//#define DEBUG_AUDIO

#include "vumeterwidget.hpp"
#include "spectrumwidget.hpp"
#include "guitest.hpp"

#define BufferSize 4096

guitest::guitest(QWidget *parent) : QMainWindow(parent)
{
    mcb = new ModemControlBlock;

    ui = new Ui::guitest;
    ui->setupUi(this);

    ui->vumeter->setMinimum(0);
    ui->vumeter->setMaximum(100);

    readSettings();

    ui->spectrum->setMCB(mcb);
    if ( mcb->Stereo )
      ui->spectrum->init(BufferSize,2,mcb->RxAudioSampleRate);
    else
        ui->spectrum->init(BufferSize,1,mcb->RxAudioSampleRate);

    setupForm();

    // fill device lists
    qInfo() << "finding available audio devices";
    cllCaptureDevices  = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    cllPlaybackDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

//    initializeAudio();

    intRxVolume = mcb->RxVolume;
    ui->ctlVol->setValue(intRxVolume);
    connect(ui->ctlVol,SIGNAL(sliderMoved(int)),this,SLOT(slotVolChgd(int)));

    ui->retranslateUi(this);
}

void guitest::changeEvent(QEvent *e)
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

void guitest::slotVolChgd(int v)
{
    // QAudioInput needs a real value
    // so we convert the int value from the
    // panel volume control to a local real value
    qreal m_vol = 0.0;

    m_vol = v / 100;
    if ( v > 1.0 ) m_vol = 1.0;
    audioInput->setVolume(m_vol);
}

void guitest:: slotReadMore()
{
    if ( !audioInput ) return;
    qint64 len = audioInput->bytesReady();
    if ( len > BufferSize ) len = BufferSize;
    qint64 l = audioIOObject->read(m_buffer->data(), len);
    if ( l > 0 )  audioIOObject->write(m_buffer->constData(), l);
    ui->spectrum->realFFT(m_buffer);
}

void guitest::slotUpdateVU()
{
    quint16 v = 0;
    v = audioIOObject->level() * 100;
    if ( v > 100 ) v = 100;
    ui->vumeter->setValue(v);
}


void guitest::initializeAudio()
{
    qInfo() << "Initializing Audio Devices";

    bool blnGoodFmt = false;
    QAudioFormat NewFormat = QAudioDeviceInfo::defaultInputDevice().preferredFormat();

    qDebug() << "Set up the audio engine with default format and test";
    ui->vumeter->setValue(0);

    m_format.setSampleType(QAudioFormat::SignedInt);  // default for now
    m_format.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
    m_format.setChannelCount(2);
    m_format.setSampleSize(16);                       // default for now
    m_format.setSampleRate(8000);
    m_format.setCodec("audio/pcm");                   // Linear PCM codem - our only audio codec for now

    strCaptureDev = "pulse";

#ifdef DEBUG_AUDIO
    qInfo() << "Set up the audio IO device specified in settings";
    qInfo() << "- - - is the setting 'default' ?";
#endif
    if ( strCaptureDev == "" ||
         strCaptureDev == "default"
       )
    {
#ifdef DEBUG_AUDIO
        qInfo() << "Setting is 'default' ?";
#endif
        devCaptureDevice = QAudioDeviceInfo::defaultInputDevice();
        QAudioDeviceInfo info(devCaptureDevice);
        devCaptureDevice = info;
        blnGoodFmt = info.isFormatSupported(m_format);
        if ( !blnGoodFmt )
        {
            NewFormat = info.nearestFormat(m_format);

    #ifdef DEBUG_AUDIO
            qDebug() << "Audio Format Found:";
            if ( NewFormat.byteOrder() == QAudioFormat::BigEndian )
                qDebug() << "   Byte: BIG ENDIAN";
            else
                qDebug() << "   Byte: LITTLE ENDIAN";
            qDebug() << "   Ch:" << QString("%1").arg(NewFormat.channelCount());
            qDebug() << "Codec:" << QString("%1").arg(NewFormat.codec());
            qDebug() << "   Sz:" << QString("%1").arg(NewFormat.sampleSize());
            qDebug() << " Rate:" << QString("%1").arg(NewFormat.sampleRate());
            if ( NewFormat.sampleType() == QAudioFormat::SignedInt )
                qDebug() << "  Typ: SignedInt" << "\n";
            else if ( NewFormat.sampleType() == QAudioFormat::UnSignedInt )
                qDebug() << "  Typ: UnSignedInt" << "\n";
            else if ( NewFormat.sampleType() == QAudioFormat::Float )
                qDebug() << "  Typ: Float" << "\n";
    #endif
        }
    }
    else
    {
        int x = 0;
        int y = cllCaptureDevices.count();
#ifdef DEBUG_AUDIO
        qInfo() << "No - so find the device "
                << strCaptureDev << "in the available devices";
#endif
        m_format.setSampleType(QAudioFormat::SignedInt);  // default for now
        m_format.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
        m_format.setChannelCount(2);
        m_format.setSampleSize(16);                       // default for now
        m_format.setSampleRate(8000);
        m_format.setCodec("audio/pcm");                   // Linear PCM codem - our only audio codec for now
#ifdef DEBUG_AUDIO
        qDebug() << "Setting Audio Format To:";
        if ( m_format.byteOrder() == QAudioFormat::BigEndian )
            qDebug() << "   Byte: BIG ENDIAN";
        else
            qDebug() << "   Byte: LITTLE ENDIAN";
        qDebug() << "   Ch:" << QString("%1").arg(m_format.channelCount());
        qDebug() << "Codec:" << QString("%1").arg(m_format.codec());
        qDebug() << "   Sz:" << QString("%1").arg(m_format.sampleSize());
        qDebug() << " Rate:" << QString("%1").arg(m_format.sampleRate());
        if ( m_format.sampleType() == QAudioFormat::SignedInt )
            qDebug() << "  Typ: SignedInt" << "\n";
        else if ( m_format.sampleType() == QAudioFormat::UnSignedInt )
            qDebug() << "  Typ: UnSignedInt" << "\n";
        else if ( m_format.sampleType() == QAudioFormat::Float )
            qDebug() << "  Typ: Float";
        qDebug() << "Now finding settings device in the available device list.";
#endif
        for ( x = 0; x < y; ++x )
        {
            if ( cllCaptureDevices.at(x).deviceName() == strCaptureDev ) break;
#ifdef DEBUG_AUDIO
            qDebug() << "Dev:" << cllCaptureDevices.at(x).deviceName();
#endif
        }
        if ( x >= y )
            devCaptureDevice = QAudioDeviceInfo::defaultInputDevice();
        else
            devCaptureDevice = cllCaptureDevices.at(x);
        strCaptureDev = devCaptureDevice.deviceName();
        mcb->CaptureDevice = strCaptureDev;
#ifdef DEBUG_AUDIO
        qDebug() << "Dev" << QString("%1").arg(x)
                 << ":" << QString("%1").arg(y)
                 << devCaptureDevice.deviceName()
                 << "selected from settings";
#endif
    }
#ifdef DEBUG_AUDIO
    qDebug() << "Note: Format must be set before any other IO function"
             << "is called (like start or writeData() )";
    qDebug() << "Format set continuing to init Audio";
#endif
    audioIOObject = new AudioInputIO;
    audioIOObject->setAudioFormat(m_format); // set the format into the IO object

    connect(audioIOObject, SIGNAL(update()),    this, SLOT(slotUpdateVU()));
    connect(audioIOObject, SIGNAL(readyRead()), this, SLOT(slotReadMore()));

    createAudioInput();
}

void guitest::createAudioInput()
{
     intRxVolume = mcb->RxVolume;
     ui->ctlVol->setValue(intRxVolume);

    audioInput = new QAudioInput(devCaptureDevice, m_format,this);
    audioInput->setVolume(intRxVolume);
    audioIOObject->start();
    audioInput->start(audioIOObject);
    if ( audioInput->error() != QAudio::NoError )
    {
#ifdef DEBUG_AUDIO
         audioInputError(audioInput->error());
        qDebug() << "AUDIO input not open ...";
#endif
        return;
    }
#ifdef DEBUG_AUDIO
    qInfo() << "Starting Audio IO Device ";
#endif
}



void guitest::audioInputError(QAudio::Error e)
{
    // to suppress warning in release output
    Q_UNUSED(e)

    switch ( e )
    {
    case QAudio::NoError:
        break;
    case QAudio::FatalError:
        qDebug() << "Fatal Error on Audio Input";
        break;
    case QAudio::IOError:
        qDebug() << "I/O Error on Audio Input";
        break;
    case QAudio::OpenError:
        qDebug() << "Open Error on Audio Input";
        break;
    case QAudio::UnderrunError:
        qDebug() << "Underrun Error on Audio Input";
        break;
    default:
        break;
    }
}


void guitest::setupForm()
{
    m_buffer = new QByteArray("\0",BufferSize);
    // set input device to the system default
    if ( strCaptureDev == "" ||
         strCaptureDev == "default"
       )
    {
        devCaptureDevice = QAudioDeviceInfo::defaultInputDevice();
        strCaptureDev    = devCaptureDevice.deviceName();
    }
}

void guitest::readSettings()
{
#ifdef DEBUG_AUDIO
    qDebug() << "Reading Settings from ardop_tnc.ini...";
#endif
    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("SOUND");
     mcb->CaptureDevice  = settings.value("CaptureDevice",  "default").toString();
     mcb->PlaybackDevice = settings.value("PlaybackDevice", "default").toString();
     mcb->RxVolume       = settings.value("RxVolume",             70).toInt();
     mcb->TxDriveLevel   = settings.value("DriveLevel",         80).toInt();
     mcb->RxSquelchLevel = settings.value("Squelch",            20).toInt();
     mcb->Stereo         = settings.value("Stereo",             false).toBool();
     mcb->CaptureClock   = settings.value("CaptureClock",       0).toInt();
     mcb->PlaybackClock  = settings.value("PlaybackClock",      0).toInt();
     mcb->RxAudioSampleRate = settings.value("RxSampleRate", 8000).toInt();
     mcb->TxAudioSampleRate = settings.value("TxSampleRate", 8000).toInt();
     intChannels    = settings.value("RxChannels",          1).toInt();
    settings.endGroup();
    // update local vars with control block values
    strCaptureDev   = mcb->CaptureDevice;
    intRxsmplrate   = mcb->RxAudioSampleRate;

    settings.beginGroup("NETWORK");
      mcb->TCPPort        = settings.value("TCPIPPort",         8515).toInt();
      mcb->TCPAddress     = settings.value("TCPIPAddress",      "127.0.0.1").toString();
      mcb->HostTCPIP      = settings.value("HostTCPIP",         true).toBool();
    settings.endGroup();

    settings.beginGroup("STATION");
      mcb->Callsign        = settings.value("Callsign",         "unknown").toString();
      mcb->Password        = settings.value("LoginPassword",    "").toBool();
      mcb->CWID            = settings.value("EnableCWID",       false).toBool();
      mcb->SecureHostLogin = settings.value("SecureHostLogin",  false).toBool();
      appver               = settings.value("Version", "").toString();
    settings.endGroup();

    settings.beginGroup("TNC");
     mcb->SerBaud        = settings.value("SerialBaud",         19200).toInt();
     mcb->SerCOMPort     = settings.value("SerialCOMPort",      "COM3").toString();
     mcb->HostPairing    = settings.value("HostPairing",        "").toString();
     mcb->HostSerial     = settings.value("HostSerial",         false).toBool();
     mcb->HostBlueTooth  = settings.value("HostBlueTooth",      false).toBool();
     mcb->StartMinimized = settings.value("StartMinimized",     false).toBool();
     mcb->DebugLog       = settings.value("DebugLog",           false).toBool();
     mcb->CommandTrace   = settings.value("CommandTrace",       false).toBool();
     mcb->LeaderLength   = settings.value("LeaderLenth",        0).toInt();
     mcb->TrailerLength  = settings.value("TrailerLength",      0).toInt();
     mcb->AccumulateStats = settings.value("AccumStats",        false).toBool();
     mcb->DisplayWaterfall = settings.value("DispWaterfall",    true).toBool();
     mcb->DisplaySpectrum  = settings.value("DispSpectrum",     false).toBool();
     mcb->TuningRange    = settings.value("TuningRange",        100).toInt();
     //RCB->RadioControl   = settings.value("Enbl Radio Cont",    false).toBool();
     mcb->FECRepeats     = settings.value("FECRepeats",         1).toInt();
     mcb->FECMode        = settings.value("FECMode",            "").toString();
     mcb->FECId          = settings.value("FECId",              false).toBool();
     mcb->ARQConReqRepeats = settings.value("ARQConRepReq",     5).toInt();
     mcb->ProtocolMode   = settings.value("ProtocolMode",       "").toString();
     mcb->ARQTimeout     = settings.value("ARQTimeout",         100).toInt();
     mcb->SlowCPU        = settings.value("SlowCPU",            false).toBool();
    settings.endGroup();

/*
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
*/
#ifdef DEBUG_AUDIO
    qDebug() << "Reading Settings complete...returning";
#endif
}


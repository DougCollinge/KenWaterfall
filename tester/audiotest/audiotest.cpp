#include <QtCore/QDebug>
#include <QtMultimedia/QAudio>

#include "audiotest.hpp"

audiotest::audiotest(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::audiotest;
    ui->setupUi(this);

    ui->ledSND->setEnabled(false);
    initializeAudio();g
}

void audiotest::changeEvent(QEvent *e)
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

void audiotest::slotUpdateVU()
{
    static qreal previousvalue = 0.0;

    const qreal ffactor = .1;
    qreal v = 0.0;
    qreal newv = 0.0;

//    v = audioIOObject->level() * 100.0;
    v = audioIOObject->level();
    v = v*v*10000;

//    if ( v > 100.0 ) v = 100.0;
//    newv = ((previousvalue + v) / v) + v;

    newv = (v + ffactor*previousvalue)/(1+ffactor);
//    newv = v;
#ifdef DEBUG_AUDIO
//    qDebug() << "Lvl: " << QString("%1").arg(newv);
#endif
    ui->vumeter->setValue(int(newv));
    previousvalue = newv;
}

void audiotest::slotReadMore()
{
    if ( !audioInput ) return;
    qint64 len = audioInput->bytesReady();
    if ( len > BufferSize ) len = BufferSize;
    qint64 l = m_input->read(m_buffer.data(), len);
    if ( l > 0 )  audioIOObject->write(m_buffer.constData(), l);
    slotUpdateVU();
}

void audiotest::initializeAudio()
{
    qInfo() << "Initializing Audio Devices";

    bool blnGoodFmt = false;
    QAudioFormat NewFormat = QAudioDeviceInfo::defaultInputDevice().preferredFormat();

    qDebug() << "Set up the audio engine with default format and test";

    format.setSampleType(QAudioFormat::SignedInt);  // default for now
    format.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
    format.setChannelCount(2);
    format.setSampleSize(16);                       // default for now
    format.setSampleRate(8000);
    format.setCodec("audio/pcm");                   // Linear PCM codem - our only audio codec for now

    auddevinfo = QAudioDeviceInfo::defaultInputDevice();
    QAudioDeviceInfo info(auddevinfo);

    qInfo() << "Setting audio IO device to nearest format. ";
    qDebug() << "Note: Format must be set before any other IO function"
             << "is called (like start or writeData() )";

    blnGoodFmt = info.isFormatSupported(format);
    // test for valid formats for selected device
    if ( !blnGoodFmt )
    {
        qDebug() << "Audio Format Rejected:";
        if ( format.byteOrder() == QAudioFormat::BigEndian )
            qDebug() << "   Byte: BIG ENDIAN";
        else
            qDebug() << "   Byte: LITTLE ENDIAN";
        qDebug() << "   Ch:" << QString("%1").arg(format.channelCount());
        qDebug() << "Codec:" << QString("%1").arg(format.codec());
        qDebug() << "   Sz:" << QString("%1").arg(format.sampleSize());
        qDebug() << " Rate:" << QString("%1").arg(format.sampleRate());
        if ( format.sampleType() == QAudioFormat::SignedInt )
            qDebug() << "  Typ: SignedInt" << "\n";
        else if ( format.sampleType() == QAudioFormat::UnSignedInt )
            qDebug() << "  Typ: UnSignedInt" << "\n";
        else if ( format.sampleType() == QAudioFormat::Float )
            qDebug() << "  Typ: Float" << "\n";

        NewFormat = info.nearestFormat(format);

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
    }

    ui->vumeter->setValue(0);
    audioIOObject = new AudioIO(this);
    if ( !blnGoodFmt )   format = NewFormat;
    audioIOObject->setAudioFormat(format);
    if ( audioIOObject->errorString() != "" )
        qDebug() << "Audio IO object error on format set:" << audioIOObject->errorString();
    // When the update signal is received from the
    // IO Device, update the VU Meter using level()
    connect(audioIOObject, SIGNAL(update()),    this,SLOT(slotUpdateVU()));
    connect(audioIOObject, SIGNAL(readyRead()), this,SLOT(slotReadMore()));

    qInfo() << "Input Dev:" << auddevinfo.deviceName();
    createAudioInput();
}

void audiotest::createAudioInput()
{
     quint16 intRxVolume = 85;

     ui->ctlVol->setValue(intRxVolume);

     qInfo() << "Creating Audio Input Device with " << auddevinfo.deviceName();

     audioInput = new QAudioInput(auddevinfo, format,this);
     audioInput->setVolume(intRxVolume);
     m_input = audioInput->start();
     audioIOObject->start();
     if ( audioIOObject->errorString() != "" )
         qDebug() << "IO object start error " << audioIOObject->errorString();
     audioInput->start(audioIOObject);
     if ( !audioIOObject->isOpen() )
     {
         qDebug() << "AUDIO input not open ...";
         ui->ledSND->setEnabled(false);
         return;
     }
     else
     {
         ui->ledSND->setEnabled(true);
         return;
     }
}

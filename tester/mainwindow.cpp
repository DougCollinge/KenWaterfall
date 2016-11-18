#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtMultimedia/QAudioFormat>

#include "globals.hpp"
#include "bufferobject.hpp"
#include "audioio.hpp"
#include "vumeterwidget.hpp"
#include "waterfallwidget.hpp"

#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    sem.release(1);
    fps = 20;

    ui = new Ui::MainWindow;
    ui->setupUi(this);

    ui->vumeter->setValue(0);
    ui->vumeter->setMinimum(0);
    ui->vumeter->setMaximum(100);

    rxBuffer = new BufferObject(this);
    rxBuffer->setBufSize(4096);
    bufsize= rxBuffer->getBufSize();

    qDebug() << "Initializing Waterfall Object";
    ui->waterfall->initialize();

    qDebug() << "Reading stored settings";
    readSettings();

    qDebug() << "Setting default format";
    QAudioFormat format;
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setSampleRate(8000);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);

    qDebug() << "Checking settings for input device";
    devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    m_device = QAudioDeviceInfo::defaultInputDevice();
    qDebug() << "Default will be used if none set";

    for ( int x = 0; x < devices.count(); ++x )
    {
        if ( strCaptureDev == devices[x].deviceName() )
        {
            m_device = devices[x];
            break;
        }
    }

    QAudioDeviceInfo info(m_device);

    if ( !info.isFormatSupported(format) )
    {
        qWarning() << "Default format not supported - trying to use nearest";
        format = info.nearestFormat(format);
    }

    qDebug() << "Creating new IO Object";
    audioIOObject = new AudioIO(this);

    audioIOObject->setAudioFormat(format);
    audioIOObject->setRxBuffer(rxBuffer);

    qDebug() << "Starting IO Object with format set";
    audioIOObject->start();
    qDebug() << "Connecting IO Object to local slot";
    connect(audioIOObject,SIGNAL(update()),this,SLOT(slotUpdateFFT()));

    qDebug() << "Creating New Input Object";
    audioInput = new QAudioInput(m_device, format, this);
    audioInput->start(audioIOObject);
    audioInput->setVolume(60.0);

    audioInput->stop();
    audioIODevice = audioInput->start();

    if ( !audioIODevice->isOpen() )   qDebug() << "input not open ...";

    qDebug() << "Connecting IODevice Object readyRead() to local readmore() slot";
    connect(audioIODevice, SIGNAL(readyRead()), this,SLOT(readMore()));

    qDebug() << "Creating Timer Object";
    QTimer* qTimer = new QTimer(this);
    connect(qTimer, SIGNAL(timeout()), this, SLOT(slotUpdateFFT()));
    qTimer->start(1000 / fps);  // ~50ms for 20 fps
    qDebug() << "Timer started -- construction complete";
}

MainWindow::~MainWindow()
{
    audioInput->stop();
    audioIODevice->close();
    audioIOObject->stop();

    try
    {
        delete audioInput;
        delete audioIODevice;
        delete audioIOObject;
        delete rxBuffer;
    }
    catch (...)
    {
        qFatal("Error deleting objects in deconstructor");
    }

    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
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


void MainWindow::slotUpdateVU()
{
    quint16 v = 0;
//    qDebug() << "Snd Lvl:" << QString("%1").arg(audioIODevice->level());
    v = audioIOObject->level() * 100;
    if ( v > 100 ) v = 100;
//    qDebug() << "Lvl:" << QString("%1").arg(v);
    ui->vumeter->setValue(v);
    ui->vumeter->update();
}


void MainWindow::slotUpdateFFT()
{
    qDebug() << "WF Update called";
    ui->waterfall->updateWaterfall(rxBuffer);
}

void MainWindow::readMore()
{
    if ( !audioIOObject )  return;
    // qDebug() << "processing audio"
    qint64 len = audioInput->bytesReady();
    if ( len > bufsize ) len = 4096;
    QByteArray data = audioIODevice->readAll();
    // get buffer length
    quint32 bl = rxBuffer->length();
    // get data size
    quint32 ds = data.size();
    // get starting slot
    quint32 st = bl - (bl - rxBuffer->NumUsedBytes);
    // if start bucket equal buffer length start over at zero
    if ( st == bl ) st = 0;
    // for start bucket to data size dump data into buffer
    for ( int i = st; i < ds; ++i )
    {
        rxBuffer->mutex.lock();
        if ( (quint32)rxBuffer->NumUsedBytes == bufsize )
            rxBuffer->bufferNotFull.wakeAll();
        else
            rxBuffer->bufferNotFull.wait(&rxBuffer->mutex);

        rxBuffer->buffer[st + i] = reinterpret_cast<const char*>(data.at(i));
        ++rxBuffer->NumUsedBytes;
        rxBuffer->mutex.unlock();
    }
    if ( ds > 0 ) audioIODevice->write(rxBuffer->constData(), ds);
    if ( !(quint32(rxBuffer->NumUsedBytes) == rxBuffer->getBufSize()) )
    {
        ui->waterfall->updateWaterfall(rxBuffer);
        rxBuffer->bufferNotFull.wait(&rxBuffer->mutex,10);
        rxBuffer->bufferNotEmpty.wakeAll();
    }
}

void MainWindow::readSettings()
{
    qInfo() << "Reading settings from .ini";
    QDir dir;
    QString path = "./ardop_tnc.ini";
    //bool ok = dir.exists(path);
    //if ( !ok ) saveSettings();

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("SOUND");
      strCaptureDev      = settings.value("CaptureDevice",  "default").toString();
      strPlaybackDev     = settings.value("PlaybackDevice", "default").toString();
      MCB.TxDriveLevel   = settings.value("DriveLevel",     80).toInt();
      MCB.RxSquelchLevel = settings.value("Squelch",        20).toInt();
      MCB.Stereo         = settings.value("Stereo",         false).toBool();
      MCB.CaptureClock   = settings.value("CaptureClock",   0).toInt();
      MCB.PlaybackClock  = settings.value("PlaybackClock",  0).toInt();
    settings.endGroup();

     settings.beginGroup("SPECTRUM");
      maxdb = settings.value("maxdb",-25).toInt();
      range = settings.value("range",35).toInt();
      avg   = settings.value("avg",0.90).toDouble();
     settings.endGroup();
}

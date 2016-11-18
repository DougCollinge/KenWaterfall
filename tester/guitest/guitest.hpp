#ifndef GUITEST_HPP
#define GUITEST_HPP
#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtWidgets/QMainWindow>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>

#include "modem.hpp"
#include "audioinputio.hpp"

#include "ui_guitest.h"

class guitest : public QMainWindow
{
    Q_OBJECT

public:
    explicit guitest(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotReadMore();
    void slotUpdateVU();
    void slotVolChgd(int v);

private:
    // Integers
    quint8   intChannels;
    quint16 intRxVolume;
    quint32  length;
    quint32  slices;
    quint32  intRxsmplrate;

    QList<QAudioDeviceInfo> cllCaptureDevices;  //!< CaptureDevicesCollection
    QList<QAudioDeviceInfo> cllPlaybackDevices; //!< PlaybackDevicesCollection

    QString    strCaptureDev;
    QString    appver;


    // Qt Library Audio Objects
    QAudioFormat     stcSCFormat;
    QAudioFormat     m_format;
    QAudioDeviceInfo devCaptureDevice;
    QAudioDeviceInfo devSelectedPlaybackDevice;  // Device Info Selected

    QByteArray   *m_buffer;

    QAudioInput  *audioInput;
    QAudioOutput *audioOutput;
    QIODevice    *m_input;
    // Sub-classed local object
    AudioInputIO *audioIOObject;  //!< \sa AudioInputIO
    // Modem parent object
    ARDOPModem   *objMain;
    // Local Control Block objects used by reference elsewhere
    ModemControlBlock *mcb;

    Ui::guitest *ui;

    void initializeAudio();
    void createAudioInput();
    void setupForm();
    void readSettings();
    void audioInputError(QAudio::Error e);
};

#endif // GUITEST_HPP

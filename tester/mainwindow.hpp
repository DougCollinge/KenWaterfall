#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include <QtCore/QIODevice>
#include <QtCore/QByteArray>
#include <QtCore/QSemaphore>
#include <QtWidgets/QMainWindow>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>

#include "ui_mainwindow.h"

class AudioIO;
class BufferObject;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotUpdateVU();
    void slotUpdateFFT();
    void readMore();

private:
    quint32 bufsize;
    int     fps;
    int     maxdb;
    int     range;
    double  avg;

    QString strCaptureDev;
    QString strPlaybackDev;

    QAudioDeviceInfo m_device;
    QByteArray       m_buffer;

    QSemaphore     sem;

    QList<QAudioDeviceInfo> devices;

    Ui::MainWindow *ui;

    QAudioInput    *audioInput;
    QIODevice      *audioIODevice;

    AudioIO        *audioIOObject;
    BufferObject   *rxBuffer;

    void readSettings();
};

#endif // MAINWINDOW_HPP

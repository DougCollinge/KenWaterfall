#ifndef AUDIOTEST_HPP
#define AUDIOTEST_HPP
#include <QtWidgets/QMainWindow>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>

#include "audioio.hpp"
#include "ui_audiotest.h"


class audiotest : public QMainWindow
{
    Q_OBJECT

public:
    explicit audiotest(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotUpdateVU();
    void slotReadMore();

private:
    QByteArray     m_buffer;

    QAudioFormat   format;

    QAudioDeviceInfo auddevinfo;

    QAudioInput   *audioInput;

    QIODevice     *m_input;

    AudioIO       *audioIOObject;

    Ui::audiotest *ui;

    void initializeAudio();
    void createAudioInput();
};

#endif // AUDIOTEST_HPP

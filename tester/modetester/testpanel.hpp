#ifndef TESTPANEL_HPP
#define TESTPANEL_HPP
#include <QtCore/QDebug>
#include <QtCore/QVarLengthArray>
#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>

class TNCPanel;
class Channel;

#include "globals.hpp"
#include "ui_testpanel.h"

namespace Ui {
class TestPanel;
}

class TestPanel : public QDialog
{
    Q_OBJECT

public:
    explicit TestPanel(QWidget *parent = 0);
    ~TestPanel();

    void setMCB();

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotPlayWave();  // Handles btnPlayWave.Click
    void slotFECTest();   // Handles btnFECTest.Click
    void slotFECAbort();  // Handles btnFECAbort.Click
    void slotReadSerialPortData(); // Handles Serial Port Data reads
    void slotSerialPortError();    // Handles Serial Port errors
    void slotWriteSerialData();    // Writes data to the serial port console for test

private:
    quint32 intBreakCounts;
    quint32 intTestFrameCorrectCnt;

    QVarLengthArray<qint32> intSamples;

    QByteArray bytEncodedBytes;

    QString strFilename;
    QString strMsg;

    Ui::TestPanel*     ui;
    ModemControlBlock* MCB;
    RadioControlBlock* RCB;

    TNCPanel*    objMain;
    Channel*     objChannel;
    QSerialPort* objSerial;

    void connectObjects();
    void closeSerialPort();
    void readSettings();
    void writeSettings();
    QVarLengthArray<qint32> AddNoise(QVarLengthArray<qint32> , qint32);
};

#endif // TESTPANEL_HPP

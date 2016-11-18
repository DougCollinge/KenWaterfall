#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtWidgets/QMessageBox>


#include "utils.hpp"
#include "modulator.hpp"
#include "channel.hpp"
#include "ardopprotocol.hpp"
#include "ardopmodem.hpp"
#include "tncpanel.hpp"

#include "testpanel.hpp"

TestPanel::TestPanel(TNCPanel *parent) : QDialog(parent)
{
    ui = new Ui::TestPanel;
    ui->setupUi(this);

    objMain = parent;
    objSerial = objMain->objHI->objSerial;
    objChannel = new Channel(this);

    // Add any initialization after the InitializeComponent() call.
    objMain->blnInTestMode = true;

    strMsg = "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.\n" +
             "KN6KB: The quick brown fox jumped over the lazy dog's back.\n" +
             "Now is the time for all good men to come to the aid of their country.";


    connectObjects();
    ui->lblFECInfo->setText("Mode: " + MCB->FECMode + " Rpt = " + QString("%1").arg(MCB->FECRepeats));
}

TestPanel::~TestPanel()
{
    delete ui;
}

void TestPanel::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TestPanel::closeEvent(QCloseEvent *)
{
    objMain->blnInTestMode = false;
}

void TestPanel::UpdateFrameCounter(qint32 intCount)
{
    ui->lblCount->setText(QString("%1").arg(intCount));
    this->repaint();
}

void TestPanel::setMCB(ModemControlBlock mcb, RadioControlBlock rcb)
{
    MCB = mcb;
    RCB = rcb;
}

void TestPanel::slotReadSerialPortData()
{
    QByteArray data = objSerial->readAll();
    ui->console->document()->   data);
}

void TestPanel::slotSerialPortError()
{
    if (objSerial->error() == QSerialPort::ResourceError)
    {
          QMessageBox::critical(this, tr("Critical Error"), objSerial->errorString());
          closeSerialPort();
    }
    else if (objSerial->error() == QSerialPort::BreakConditionError)
    { }
    else if (objSerial->error() == QSerialPort::DeviceNotFoundError)
    {
        QMessageBox::critical(this, tr("Critical Error"), objSerial->errorString());
        closeSerialPort();
    }
    else if (objSerial->error() == QSerialPort::FramingError)
    {
        ui->console->document()->setPlainText(tr("Critical Error") + objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::NotOpenError)
    {
        QMessageBox::critical(this, tr("Critical Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::OpenError)
    {
        QMessageBox::critical(this, tr("Error On Open"), objSerial->errorString());
        closeSerialPort();
    }
    else if (objSerial->error() == QSerialPort::ParityError)
    {
        QMessageBox::critical(this, tr("Parity Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::PermissionError)
    {
        QMessageBox::critical(this, tr("Permissions Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::ReadError)
    {
        QMessageBox::critical(this, tr("Read Data Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Resourse Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::SerialPortError)
    {
        QMessageBox::critical(this, tr("Serial Port Error"), objSerial->errorString());
    }
    else if (objSerial->error() == QSerialPort::TimeoutError)
    {
        QMessageBox::critical(this, tr("Port Timeout Error"), objSerial->errorString());
        closeSerialPort();
    }
    else if (objSerial->error() == QSerialPort::UnsupportedOperationError)
    {
        QMessageBox::critical(this, tr("Unsupported Operation"), objSerial->errorString());
        closeSerialPort();
    }
    else if (objSerial->error() == QSerialPort::WriteError)
    {
        ui->console->document()->setPlainText(tr("Error On WRITE operation") + objSerial->errorString());
    }
    else
    {
        if (objSerial->error() == QSerialPort::UnknownError)
            QMessageBox::critical(this, tr("Unexpected Error"), objSerial->errorString());
        closeSerialPort();
    }
}

void TestPanel::slotWriteSerialData()
{

}

void TestPanel::slotFECTest()
{
    // Set up to send 3 data frames repeated twice (9 total) using 1KHz 2 Car 4FSK FEC mode.
    clearTuningStats();
    clearQualityStats();
    InitializeConnection();
    QByteArray bytEncodedData;
    bytEncodedData = GetBytes(strMsg.mid(0, 256)); // Enough data for 2 full frames of 2 Car 4FSK
    MCB->GridSquare = "EL98pf";
    objMain->objProtocol->ClearDataToSend();
    objMain->objProtocol->addDataToDataToSend(bytEncodedData);
    QByteArray bytNull;
    objMain->objProtocol->StartFEC(bytNull, MCB->FECMode, MCB->FECRepeats, true);
}

void TestPanel::slotFECAbort()
{
    objMain->objProtocol.Abort();
}

void TestPanel::slotPlayWave()
{
    ui->lblCount->setText(QString("%1").arg(ui->nudRepeats->value()));
    InitializeConnection();
    bytEncodedBytes.clear();
    intBreakCounts = 0;
    intTestFrameCorrectCnt = 0;

    // BREAK
    // for 4FSK modulation
    if ( ui->rdoBreak->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->Encode4FSKControl(FRAME_4FSKBREAK, strFilename, 255);

        intSamples = objMain->objMod->Mod4FSKFrameID(FRAME_4FSKBREAK, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // END
    if ( ui->rdoEND->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->Encode4FSKControl(FRAME_END, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKFrameID(FRAME_END, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }


    // DISC
    if ( ui->rdoDISC->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->Encode4FSKControl(FRAME_DISC, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKFrameID(FRAME_DISC, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats.value());
    }


    // CONREJBUSY
    if ( ui->rdoCONREJBUSY->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->Encode4FSKControl(FRAME_CONREJBUSY, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKFrameID(FRAME_CONREJBUSY, bytEncodedBytes);
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // DataNAK
    if ( ui->rdoDataNAK->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeDATANAK(FRAME_CONREQ2, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKFrameID(bytEncodedBytes.at(0), bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN.value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats.value());
    }

    //DataACK
    if ( ui->rdoDataACK->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeDATAACK(FRAME_CARR_DATA0, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKFrameID(bytEncodedBytes.at(0), bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // ConREQ
    if ( ui->rdoCONREQ200->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeARQConRequest("KN6KB", "W1AW", "200M", strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONREQ1, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }
    else if ( ui->rdoCONREQ500->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeARQConRequest("KN6KB", "W1AW", "500M", strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONREQ2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }
    else if ( ui->rdoCONREQ1000->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeARQConRequest("KN6KB", "W1AW", "1000M", strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONREQ3, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }
    else if ( ui->rdoCONREQ2000.isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeARQConRequest("KN6KB", "W1AW", "2000M", strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONREQ3, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // ConACK with timing:
    if ( ui->rdoCONNAK200T->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeConACKwTiming(FRAME_CONACK200, 1000, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONACK200, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    if ( ui->rdoCONACK500T->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeConACKwTiming(FRAME_CONACK500, 100, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONACK500, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    if ( ui->rdoCONACK1000T->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeConACKwTiming(FRAME_CONACK1K, 100, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONACK1K, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    if ( ui->rdoCONACK2000T->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->EncodeConACKwTiming(FRAME_CONACK2K, 100, strFilename, 255);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_CONACK2K, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    //200 Hz modes

    // 64 Data Byte + 32 RS 4PSK 200 Hz data frame
    if ( ui->rdo4PSK200_64->isChecked() )
    {
        QDateTime dttStartTime = QDateTime::currentDateTime();
        QByteArray bytMsg = GetBytes(strMsg.mid(0, FRAME_OK0));  // modified from 72 data bytes per frame 12/2/2014
        bytEncodedBytes = GetBytes(strMsg.mid(0, FRAME_OK0));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_OK0, bytMsg, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_OK0, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 16 Byte 4PSK 200 Hz data frame
    if ( ui->rdo4PSK200_16->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 16));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_OK2, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_OK2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 108 Byte 8PSK 200 Hz data frame
    if ( ui->rdo8PSK200_108->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 108));
        bytEncodedBytes = objMain->objMod.EncodePSK_QAM(FRAME_OK4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod.ModPSK(FRAME_OK4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 32 byte 4FSK 200Hz data frame
    if ( ui->rdo4FSK200_32->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 32));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_4FSK_DATA0, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_4FSK_DATA0, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 16 byte 4FSK 200Hz data frame
    if ( ui->rdo4FSK200_16->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 16));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_4FSK_DATA2, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_4FSK_DATA2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 24 byte 8FSK 200Hz data frame
    if ( ui->rdo8FSK200_24->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 24));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_8FSK_DATA0, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod8FSKData(FRAME_8FSK_DATA0, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 128 byte 16QAM 200Hz data frame QAM
    if ( ui->rdo16QAM_128->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 128));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_16QAM_128, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModQAM(FRAME_16QAM_128, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 500 Hz modes:
    // 128 byte 2 Car 4PSK 500 Hz BW frame
    if ( ui->rdo4PSK500_128->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 128));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_CONREQ2, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_CONREQ2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 216 Byte 8PSK 500 Hz data frame
    if ( ui->rdo8PSK500_216->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 216));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_CONREQ4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_CONREQ4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }
    // 256 byte 16QAM 100 baud Data frame
    if ( ui->rdo16QAM500_256->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 256));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_CARR_DATA4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModQAM(FRAME_CARR_DATA4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 64 byte 4FSK 500Hz data frame
    if ( ui->rdo4FSK500_64->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 64));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_4FSK_DATA4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_4FSK_DATA4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 32 byte 4FSK 500Hz data frame
    if ( ui->rdo4FSK500_32->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 32));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_4FSK_DATA6, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_4FSK_DATA6, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 32 byte 16FSK 500Hz data frame
    if ( ui->rdo16FSK500_32->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 32));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_CONACK500, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod16FSKData(FRAME_CONACK500, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 16 byte 16FSK 500Hz data frame
    if ( ui->rdo16FSK500_16->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 16));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_16FSK500_16, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod16FSKData(FRAME_16FSK500_16, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 1000 Hz modes:
    // 256 byte 4 Car 4PSK 1000 Hz BW frame
    if ( ui->rdo4PSK1000_256->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 256));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_1K_DATA0, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_1K_DATA0, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 480 byte 4PSK 167 baud Data frame
    if ( ui->rdo4PSK1000_480->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 480));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_4FSK_DATA4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_1K_DATA4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 432 Byte 8PSK 500 Hz data frame
    if ( ui->rdo8PSK1000_432->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 432));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_1K_DATA2, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_1K_DATA2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 636 Byte 8PSK 1000 Hz data frame
    if ( ui->rdo8PSK1000_636->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 636));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_1K_DATA6, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_1K_DATA6, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 128 byte 2 car 4FSK
    if ( ui->rdo4FSK1000_128->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 128));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_1K_DATA8, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_1K_DATA8, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 2000 Hz Modes:
    // 512 byte 8 Car 4PSK 2000 Hz BW frame
    if ( ui->rdo4PSK2000_512->isChecked() )
    {
        QDateTime dttTimeTest = QDateTime::currentDateTime();
        QByteArray bytMsg = GetBytes(strMsg.mid(0, 512));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_2K_DATA0, bytMsg, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_2K_DATA0, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 960 byte 4PSK 167 baud Data frame
    if ( ui->rdo4PSK2000_960->isChecked() )
    {
        bytEncodedBytes = GetBytes((strMsg + strMsg).mid(0, 960));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_2K_DATA4, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_2K_DATA4, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 864 Byte 8PSK 2000 Hz data frame
    if ( ui->rdo8PSK2000_864->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 864));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_2K_DATA2, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_2K_DATA2, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 1272 Byte 8PSK 2000 Hz data frame
    if ( ui->rdo8PSK2000_1272->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 1272));
        bytEncodedBytes = objMain->objMod->EncodePSK_QAM(FRAME_2K_DATA6, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->ModPSK(FRAME_2K_DATA6, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 256 byte 4 car 4FSK
    if ( ui->rdo4FSK2000_256->isChecked() )
    {
        bytEncodedBytes = GetBytes(strMsg.mid(0, 256));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_2K_DATA8, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_2K_DATA8, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 600 byte 1 Car 4FSK 600 bd
    if ( ui->rdo4FSK2000_600->isChecked() )
    {
        // Temp 600 byte 4FSK 600 Baud test (experimental)
        bytEncodedBytes = GetBytes(strMsg.mid(0, 590));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_2K_DATA9, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSK600BdData(FRAME_2K_DATA9, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // 200 byte 1 Car 4FSK 600 bd
    if ( ui->rdo4FSK2000_200->isChecked() )
    {
        // Temp 200 byte 4FSK 600 Baud test (experimental)
        bytEncodedBytes = GetBytes(strMsg.mid(0, 190));
        bytEncodedBytes = objMain->objMod->EncodeFSKData(FRAME_2K_DATA10, bytEncodedBytes, strFilename);
        intSamples = objMain->objMod->Mod4FSK600BdData(FRAME_2K_DATA10, bytEncodedBytes);
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain.SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // ID Frame
    if ( ui->rdoIDFrame->isChecked() )
    {
        bytEncodedBytes = objMain->objMod->Encode4FSKIDFrame("KN6KB", "EL98pf", strFilename);
        intSamples = objMain->objMod->Mod4FSKData(FRAME_ID, bytEncodedBytes);
        if ( MCB->CWID )
        {
            intSamples.reserve(intSamples.length() + 4800);
            QVarLengthArray<qint32> intCWID;
            intCWID.clear();
            qint32 intPtr = intSamples.length();
            objMain->objMod->CWID("DE " + MCB->Callsign, intCWID, false);
            intSamples.reserve(intSamples.length() + intCWID.length());
            ArrayCopy(intCWID, 0, intSamples, intPtr, intCWID.length());
        }
        intSamples = AddNoise(intSamples, ui->nudSN->value());
        objMain->SendTestFrame(intSamples, strFilename, ui->nudRepeats->value());
    }

    // Experimental Sounding
    if ( ui->rdoSounding->isChecked() )
    {
        // intSamples = objMain.objMod.ModSounder(&HFF)
        objMain->SendTestFrame(intSamples, "SOUND2K", ui->nudRepeats->value());
    }
}


QVarLengthArray<qint32> TestPanel::AddNoise(QVarLengthArray<qint32> intSamples, qint32 intSN)
{
    QVarLengthArray<double> dblInputSamples(intSamples.length() - 1);
    QVarLengthArray<double> dblSamplesWithNoise;


    for ( qint32 i = 0; i < (intSamples.length() - 1); ++i )
    {
        dblInputSamples[i] = intSamples.at(i);
    }
    dblSamplesWithNoise = objChannel->AddPinkNoiseToFloat(dblInputSamples, intSN, 0);
    QVarLengthArray<qint32> intSamplesWithNoise(dblSamplesWithNoise.length() - 1);
    for ( qint32 i = 0; i < (dblSamplesWithNoise.length() - 1); ++i )
    {
        intSamplesWithNoise[i] = dblSamplesWithNoise.at(i);
    }
    return intSamplesWithNoise;
}


void TestPanel::connectObjects()
{
    connect(ui->btnPlayWav,SIGNAL(clicked(bool)),this,SLOT(slotPlayWave()));
    connect(ui->btnFECTest,SIGNAL(clicked(bool)),this,SLOT(slotFECTest()));
    connect(ui->btnFECAbort,SIGNAL(clicked(bool)),this,SLOT(slotFECTest()));

    connect(ui->console, SIGNAL(textChanged()), this, SLOT(slotWriteSerialData()));
    connect(ui->btnClose, SIGNAL(clicked(bool)),this,SLOT(slotCloseSerialPort()));
    connect(ui->btnSerOpen, SIGNAL(clicked(bool)),this,SLOT(slotOpenSerialPort()));

    connect(objSerial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, SLOT(slotSerialPortError()));
    connect(objSerial, SIGNAL(readyRead()), this, SLOT(slotReadSerialPortData()));
}

void TestPanel::openSerialPort()
{
      objSerial->setPortName(RCB->CtrlPort);
      objSerial->setBaudRate(RCB->CtrlPortBaud);
      objSerial->setDataBits(8);
      objSerial->setParity(QSerialPort::NoParity);
      objSerial->setStopBits(1);
      objSerial->setFlowControl(QSerialPort::NoFlowControl);
      if ( objSerial->open(QIODevice::ReadWrite))
      {
          ui->console->setEnabled(true);
          QString msg =
          ui->console->document()->setText("Connected to: " +
                                           RCB->CtrlPort +
                                           QString("%1").arg(RCB->CtrlPortBaud) +
                                           QString("%1").arg(8) +
                                           "NoParity" +
                                           QString("Stop: %1").arg(1) +
                                           "NoFlowControl");
      }
      else
      {
          ui->console->document()->setText(tr("Open error"));
      }
}

void TestPanel::closeSerialPort()
{
      if ( objSerial->isOpen() )  objSerial->close();
      ui->console->setEnabled(false);
}

void TestPanel::readSettings()
{
    QDir dir;
//    bool ok = dir.exists(qApp->applicationDirPath() + "/ardop_tnc.ini");
//    if ( !ok ) writeSettings();
    QSettings settings(qApp->applicationDirPath() + "/ardop_tnc.ini",QSettings::IniFormat);
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

void TestPanel::writeSettings()
{
    QSettings settings(qApp->applicationDirPath() + "/ardop_tnc.ini",QSettings::IniFormat);
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



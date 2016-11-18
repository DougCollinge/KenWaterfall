#include <QtCore/qendian.h>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtSerialPort/QSerialPortInfo>

#include "tncsetup.hpp"

class TncSetup::TncPrivate
{
public:
    const int BufferSize = 4096;

    bool    blnPlaybackDeviceOk;
    bool    blnCaptureDeviceOk;
    bool    blnCOMDeviceOk;
    bool    blnInitialized;

    quint8  intChannels;
    quint32 intSavedTuneLineHi;
    quint32 intSavedTuneLineLo;

    QString appver;
    QString strEntryCaptureDevice;
    QString strEntryPlaybackDevice;
    QString strCaptureDev;
    QString strPlaybackDev;

    QStringList modes;

    QByteArray       m_buffer;

    QAudioDeviceInfo m_device;
    QAudioFormat     m_format;

    QList<QAudioDeviceInfo> captureDevices;
    QList<QAudioDeviceInfo> playbackDevices;

    QList<QSerialPortInfo> serialPorts;

    QAudioInput *m_audioInput;
    QIODevice   *m_input;
};

TncSetup::TncSetup(QWidget *parent) : QDialog(parent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    d = new TncPrivate;

    qInfo() << "Constructing Setup Window.";

    ui = new Ui::TncSetup;
    ui->setupUi(this);
    ui->retranslateUi(this);
}

TncSetup::~TncSetup()
{
    delete ui;
    delete d;
}

void TncSetup::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TncSetup::showEvent(QShowEvent *)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

// -----------------------------------------------
//      Public Methods and Functions
// -----------------------------------------------
void TncSetup::setControlBlock(ModemControlBlock *mcb)
{
    MCB = mcb;
    readSettings();
    displaySettings();
    initForm();
    connectObjects();
}
// -----------------------------------------------
//           Private Slots
// -----------------------------------------------
void TncSetup::slotChanChg(int)
{
    d->intChannels = ui->spinChan->value();
}

void TncSetup::slotPlayDevChg(int ndx)
{
    Q_UNUSED(ndx)

    QString Playdev = ui->cmbPlayback->currentText();
    MCB->PlaybackDevice = Playdev.trimmed();
}

void TncSetup::slotCaptureDevChg(int ndx)
{
    ui->cboCapRates->clear();
    QString Capdev = ui->cmbCapture->currentText();
    MCB->CaptureDevice = Capdev.trimmed();
    QAudioDeviceInfo dev = d->captureDevices.at(ndx);
    QList<int> rates = dev.supportedSampleRates();
    quint16 x = 0;
    quint16 y = 0;
    y = rates.count();
    for ( x = 0; x < y; ++x )
    {
        ui->cboCapRates->addItem(QString("%1").arg(rates.at(x)));
    }
}

void TncSetup::slotCaptureRateChg(int ndx)
{
    Q_UNUSED(ndx)
    quint32 CapRate = ui->cmbCapture->currentText().toInt();
    MCB->RxAudioSampleRate = CapRate;
}

void TncSetup::slotCancel(bool)
{
    reject();
}

void TncSetup::slotTCPCheckedChg(bool ckd)
{
    ui->txtTCPAddress->setEnabled(ckd);
    ui->txtTCIPControl->setEnabled(ui->rdoTCP->isChecked());
}

void TncSetup::slotSerialCkdChg(bool ckd)
{
    ui->cmbBaud->setEnabled(ckd);
    ui->cmbCOM->setEnabled(ui->rdoSerial->isChecked());
}

void TncSetup::slotBlueToothChg(bool chg)
{
    ui->cmbPairing->setEnabled(chg);
}

void TncSetup::slotOk(bool)
{  // Save button was pressed
#ifdef DEBUG_SETUP
    qDebug() << "OK button was pressed";
#endif
    qint32 intTCPPort;

    intTCPPort = ui->txtTCIPControl->text().toInt();
    if ( (intTCPPort > 65535) || (intTCPPort <= 0) )
    {   // the value limits have been exceeded so show message
        QString msg = tr("TCPIP Control port must be an integer < 65536!");
        QString title = tr("Invalid TCP Control Port number!");
        QMessageBox(QMessageBox::Information, title, msg, QMessageBox::Ok, this);
        qWarning() << msg + title;
        return;
    }
    MCB->TCPPort = intTCPPort;
    if ( ui->chkSecureLogin->isChecked() &&
        (ui->txtPassword->text().trimmed().length() < 8)
       )
    {
        QString msg = tr("If Secure TCP Login is enabled password must be entered and be between 8 and 16 printable characters (no spaces, case sensitive)");
        QString title = tr("Improper TCP Login password!");
        QMessageBox(QMessageBox::Information, title, msg, QMessageBox::Ok, this);
        qWarning() << msg + title;
        return;
    }
    bool ok = CheckValidCallsignSyntax(ui->txtCallsign->text().trimmed().toUpper());
    if ( !ok )
    {
        QString msg = tr("Invalid call sign syntax! 3 to 7 characters (0-9, A-Z) + optional ssid -1 to -15 or -A to -Z");
        QString title = tr("Invalid Call sign syntax!");
        QMessageBox(QMessageBox::Warning, title, msg, QMessageBox::Ok, this);
        qWarning() << msg + title;
        return;
    }
    // This causes a redrawing of the tuning lines on the Spectrum display
    d->intSavedTuneLineHi = 0;
    d->intSavedTuneLineLo = 0;

    // update the Master Control Block values from the GUI selections
    //    AUDIO
#ifdef DEBUG_SETUP
    qDebug() << "Capture Device:" << ui->cmbCapture->currentText();
    qDebug() << "Playback Device:" << ui->cmbPlayback->currentText();
#endif
    MCB->CaptureDevice    = ui->cmbCapture->currentText().trimmed();
    MCB->PlaybackDevice   = ui->cmbPlayback->currentText().trimmed();
    MCB->RxVolume         = ui->nudRxVol->value();
    d->intChannels = ui->spinChan->value();
    MCB->TxDriveLevel     = ui->nudDriveLevel->value();
    QString sel = ui->cboCapRates->currentText().trimmed();
    quint64 u = sel.toInt();
    if ( u == 0 )
    {
        QString msg = tr("You must select a capture device and sample rate");
        QString title = tr("Invalid Sample Rate");
        QMessageBox(QMessageBox::Warning, title, msg, QMessageBox::Ok, this);
        qWarning() << msg << title << "Value=" << QString("%1").arg(u);
        return;
    }
    MCB->RxAudioSampleRate = u;

/* unused for now
    sel = ui->cboPlayRate->currentText().trimmed();
    quint64 v = sel.toInt();
    if ( v == 0 )
    {
        QString msg = tr("You must select a playback device and sample rate");
        QString title = tr("Invalid Sample Rate");
        QMessageBox(QMessageBox::Warning, title, msg, QMessageBox::Ok, this);
        qWarning() << msg << title << "Value=" << QString("%1").arg(v);
        return;
    }
*/
    MCB->TxAudioSampleRate = 8000; // = v;

    //    STATION
    MCB->Callsign         = ui->txtCallsign->text().trimmed().toUpper();
    MCB->Password         = ui->txtPassword->text().trimmed();
    MCB->SecureHostLogin  = ui->chkSecureLogin->isChecked();
    MCB->StartMinimized   = ui->chkStartMinimized->isChecked();
    MCB->DebugLog         = ui->chkDebugLog->isChecked();
    MCB->CommandTrace     = ui->chkCmdTrace->isChecked();

    //    CODEC
    MCB->LeaderLength     = ui->nudLeaderLen->value();
    MCB->TrailerLength    = ui->nudTrailerLen->value();
    MCB->ARQBandwidth     = ui->cmbBandwidth->currentText().toUpper().trimmed();
    //    TNC
    MCB->CWID             = ui->chkEnableCWID->isChecked();
    MCB->StartMinimized   = ui->nudSquelch->value();
    MCB->DisplaySpectrum  = ui->rdoSpectrum->isChecked();
    MCB->DisplayWaterfall = ui->rdoWaterfall->isChecked();
    MCB->RxSquelchLevel   = ui->nudSquelch->value();
    MCB->TuningRange      = ui->nudTuning->value();
    //RCB->RadioControl     = ui->chkRadioControl->isChecked();
    MCB->FECMode          = ui->cmbFECType->currentText().trimmed();
    MCB->FECRepeats       = ui->nudFECRepeats->value();
    MCB->FECId            = ui->chkFECid->isChecked();
    MCB->ARQConReqRepeats = ui->nudARQConReqRpt->value();
    MCB->ProtocolMode     = ui->cmbProtocolMode->currentText();
    MCB->ARQTimeout       = ui->nudARQTimeouts->value();
    MCB->SlowCPU          = ui->chkShowCPU->isChecked();
    // HOST INTERFACE
    if ( ui->rdoTCP->isChecked() )
    {
        MCB->TCPPort       = ui->txtTCIPControl->text().toInt();
        MCB->TCPAddress    = ui->txtTCPAddress->text().trimmed();
        MCB->HostTCPIP     = true;
        MCB->HostBlueTooth = false;
        MCB->HostSerial    = false;
    }
    else if ( ui->rdoSerial->isChecked() )
    {
        MCB->SerCOMPort    = ui->cmbCOM->currentText();
        MCB->SerBaud       = ui->cmbBaud->currentText().toInt();
        MCB->HostTCPIP     = false;
        MCB->HostBlueTooth = false;
        MCB->HostSerial    = true;
    }
    else if ( ui->rdoBlueTooth->isChecked() )
    {
        MCB->HostPairing   = ui->cmbPairing->currentText();
        MCB->HostTCPIP     = false;
        MCB->HostBlueTooth = true;
        MCB->HostSerial    = false;
    }
#ifdef DEBUG_SETUP
    qDebug() << "Saving settings";
#endif
    saveSettings();

    qInfo() << "Settings saved...restarting audio";
    accept();
}

// -----------------------------------------------
//           Private Methods
// -----------------------------------------------
// Function for checking valid call sign syntax
bool TncSetup::CheckValidCallsignSyntax(QString strCallsign)
{
    qint16 x = 0;
    QString strTestCS = "";
    QString strValidChar = "";

    strTestCS = strCallsign.trimmed().toUpper();
    x = strTestCS.lastIndexOf("-",0);
    strValidChar = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if ( x == -1 )
    { // no SSID
        if ( strTestCS.length() > 7 ||
             strTestCS.length() < 3
           )  return false;
        qint16 j = strCallsign.trimmed().length() - 1;
        for ( int i = 0; i < j; ++i )
        {
            QString c = strTestCS.mid(i, 1);
            if ( strValidChar.lastIndexOf(c) < 0 )  return false;
        }
        return true;
    }
    else
    {
        QString strCallNoSSID = strTestCS.mid(0, x).trimmed();
        if ( strCallNoSSID.length() > 7 ||
             strCallNoSSID.length() < 3
             ) return false;
        QString strSSID = strTestCS.mid(x + 1,1).trimmed();
        qint16 n = strSSID.toInt();
        if ( n >= 0 )
        {
            if ( n < 0 || n > 15 )
                return false;
            else
                return true;
        }
        if ( strSSID.length() != 1 ) return false;
        bool validChr = false;
        validChr = strSSID.compare(strSSID,"ABCDEFHIJKLMNOPQRSTUVWXYZ",Qt::CaseSensitive);
        if ( !validChr )
        {
            qWarning() << "Invalid Callsign Entry - " + strCallsign;
            return false;
        }
    }
    return true;
}

void TncSetup::initForm()
{
    // load the protocol combo box from the global vector
    for ( quint16 x = 0; x < (AllModes.count() - 1); ++x )
    {
        ui->cmbProtocolMode->addItem(AllModes.at(x));
    }
    // load the FEC frame type combo box
    ui->cmbFECType->clear();
    QStringList typ;
    typ << "Negotiated";
    typ << "ARQ";
    typ << "100";
    typ << "200";
    typ << "500";
    typ << "1000";
    typ << "1600";
    typ << "2000";
    ui->cmbFECType->addItems(typ);

    // set the audio format to a default for
    // checking nearest available format
    d->m_format.setChannelCount(1);
    d->m_format.setCodec("audio/pcm");
    if ( Q_BYTE_ORDER == Q_BIG_ENDIAN )
        d->m_format.setByteOrder(QAudioFormat::BigEndian);
    else
        d->m_format.setByteOrder(QAudioFormat::LittleEndian);
    d->m_format.setSampleRate(8000);
    d->m_format.setSampleSize(16);
    d->m_format.setSampleType(QAudioFormat::SignedInt);
    QAudioFormat f = QAudioDeviceInfo::defaultInputDevice().nearestFormat(d->m_format);
    if ( f != d->m_format ) d->m_format = f;

    d->captureDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    d->playbackDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    int x = d->captureDevices.count();
    quint16 y = 0;

    for ( quint16 y = 0; y < x; ++y )
    {
        ui->cmbCapture->addItem(d->captureDevices[y].deviceName());
    }

    x = d->playbackDevices.count() - 1;
    for ( y = 0; y < x; ++y )
    {
        qDebug() << "add to cbo: " << d->playbackDevices[y].deviceName().toLocal8Bit();
        ui->cmbPlayback->addItem(d->playbackDevices[y].deviceName().toLocal8Bit());
    }

    for ( y = 0; y < x; ++y )
    {
        ui->cmbPlayback->setCurrentIndex(y);
        qDebug() << QString("%1").arg(y) << " cbo:" << ui->cmbPlayback->currentText() << "=?" << MCB->PlaybackDevice;
        if ( ui->cmbPlayback->currentText() == MCB->PlaybackDevice ) break;
    }
    if ( y > (x - 1) ) ui->cmbPlayback->setCurrentIndex(0);

    x = d->captureDevices.count();
    for ( y = 0; y < x; ++y )
    {
        if ( ui->cmbCapture->itemText(x) == MCB->CaptureDevice ) break;
    }
    if ( y > (x - 1) ) ui->cmbCapture->setCurrentIndex(0);

    for ( y = 0; y < x; ++y )
    {
        ui->cmbCapture->setCurrentIndex(y);
        qDebug() << QString("%1").arg(y) << " cbo:" << ui->cmbCapture->currentText() << "=?" << MCB->PlaybackDevice;
        if ( ui->cmbCapture->currentText() == MCB->CaptureDevice ) break;
    }
    if ( y > (x - 1) ) ui->cmbPlayback->setCurrentIndex(0);
    ui->chkSecureLogin->setChecked(false);
}

void TncSetup::displaySettings()
{ // use MCB to setup the TNC with stored values
    ui->cboPlayRate->clear();  // will be filled on selection of capture device
    ui->cboCapRates->clear();  // will be filled on selection of playback device

    ui->cmbCOM->clear();
    ui->cmbCOM->addItem("None");

    d->serialPorts = QSerialPortInfo::availablePorts();
    quint16 y = d->serialPorts.count();
    if ( y > 0 )
    {
        for ( quint16 x = 0; x < y; ++x )
        {
            ui->cmbCOM->addItem(d->serialPorts[x].portName());
        }
    }
    ui->cmbFECType->clear();
    y = 0;
    y = d->modes.count();
    for ( quint16 i = 0; i < y; ++i )
    {
        ui->cmbFECType->addItem(d->modes[i]);
    }


    quint16 cndx = 0;

    if ( !MCB->HostTCPIP )
    {
        if ( d->blnCOMDeviceOk )
            ui->cmbPairing->setStyleSheet("QComboBox {background-color: #aaffaa;");
        else
            ui->cmbPairing->setStyleSheet("QComboBox {background-color: #ffccdd;");
    }
    // initialize all the display from the MCB values
    ui->txtTCIPControl->setText(QString("%1").arg(MCB->TCPPort));
    ui->txtCallsign->setText(MCB->Callsign);
    ui->chkStartMinimized->setChecked(MCB->StartMinimized);
    ui->chkDebugLog->setChecked(MCB->DebugLog);
    ui->chkCmdTrace->setChecked(MCB->CommandTrace);
    ui->chkEnableCWID->setChecked(MCB->CWID);
    ui->rdoTCP->setChecked(MCB->HostTCPIP);
    ui->rdoSerial->setChecked(MCB->HostSerial);
    ui->rdoBlueTooth->setChecked(MCB->HostBlueTooth);
    ui->chkAccumStats->setChecked(MCB->AccumulateStats);
    cndx = ui->cmbCapture->findText(MCB->CaptureDevice);
    ui->cmbCapture->setCurrentIndex(cndx);
    ui->txtTCPAddress->setText(MCB->TCPAddress);
    cndx = 0;
    cndx = ui->cmbBandwidth->findText(MCB->ARQBandwidth);
    ui->cmbBandwidth->setCurrentIndex(cndx);
    ui->nudDriveLevel->setValue(MCB->TxDriveLevel);
    ui->nudLeaderLen->setValue(MCB->LeaderLength);
    ui->nudTrailerLen->setValue(MCB->TrailerLength);
    ui->nudSquelch->setValue(MCB->RxSquelchLevel);
    ui->rdoSpectrum->setChecked(MCB->DisplaySpectrum);
    ui->rdoWaterfall->setChecked(MCB->DisplayWaterfall);
    ui->rdoDisabled->setChecked(!MCB->DisplayWaterfall || MCB->DisplaySpectrum);
    ui->txtPassword->setText(MCB->Password);
    ui->chkSecureLogin->setChecked(MCB->SecureHostLogin);
    ui->nudTuning->setValue(MCB->TuningRange);
    cndx = 0;
    cndx = ui->cmbPairing->findText(MCB->HostPairing);
    ui->cmbPairing->setCurrentIndex(cndx);
    ui->txtTCIPControl->setEnabled(ui->rdoTCP->isChecked());
    ui->txtTCPAddress->setEnabled(ui->rdoTCP->isChecked());
    qint16 ndx = ui->cmbBaud->findText(QString("%1").arg(MCB->SerBaud));
    ui->cmbBaud->setCurrentIndex(ndx);
    ndx = 0;
    ndx = ui->cmbCOM->findText(MCB->SerCOMPort);
    ui->cmbCOM->setCurrentIndex(ndx);
    ui->cmbPairing->setEnabled(ui->rdoBlueTooth->isChecked());
    ui->cmbCOM->setEnabled(ui->rdoSerial->isChecked());
    ui->cmbBaud->setEnabled(ui->rdoSerial->isChecked());
    bool ckd = false;
    ckd = ui->chkRadioControl->isChecked();
    ui->chkRadioControl->setChecked(ckd);
    ui->nudFECRepeats->setValue(MCB->FECRepeats);
    ndx = 0;
    ndx = ui->cmbFECType->findText(MCB->FECMode);
    ui->cmbFECType->setCurrentIndex(ndx);
    ui->chkFECid->setChecked(MCB->FECId);
    ui->nudARQConReqRpt->setValue(MCB->ARQConReqRepeats);
    ui->nudARQTimeouts->setValue(MCB->ARQTimeout);
    ndx = 0;
    ndx = ui->cmbProtocolMode->findText(MCB->ProtocolMode);
    ui->cmbProtocolMode->setCurrentIndex(ndx);
    ui->chkShowCPU->setChecked(MCB->SlowCPU);
    // Get the Windows enumerated Playback devices and add to the Playback device combo box
    d->playbackDevices.clear();
    d->playbackDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    d->blnPlaybackDeviceOk = false;
    y = 0;
    y = d->playbackDevices.count() - 1;
    for ( quint16 x = 0; x < y; ++x )
    {
        // add device name to combo box
        ui->cmbPlayback->addItem(d->playbackDevices[x].deviceName());
        // test setting value to current device name
        if ( d->playbackDevices[x].deviceName() == MCB->PlaybackDevice )
        {
            // good match so set the name to local var
            // and flag as good
            d->blnPlaybackDeviceOk = true;
            d->strEntryPlaybackDevice = MCB->PlaybackDevice;
        }
    }

    // now find it in the combo box list and set it current
    ndx = 0;
    ndx = ui->cmbPlayback->findText(MCB->PlaybackDevice);
    ui->cmbPlayback->setCurrentIndex(ndx);
    // change background color of combo box if good device.
    if ( d->blnPlaybackDeviceOk )
    {
        ui->cmbPlayback->setStyleSheet("QComboBox {background-color: #ccffcc;}");
    }
    else
    {
        ui->cmbPlayback->setStyleSheet("QComboBox {background-color: #ddccaa;}");
    }

    ui->nudRxVol->setValue(MCB->RxVolume);
    ui->nudDriveLevel->setValue(MCB->TxDriveLevel);

    // Get the Windows enumerated Capture devices and
    // add to the Capture device combo box
    d->captureDevices.clear();
    d->captureDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    d->blnCaptureDeviceOk = false;
    y = d->captureDevices.count();
    for ( quint16 x = 0; x < y; ++x )
    {
        ui->cmbCapture->addItem(d->captureDevices[x].deviceName());
        if ( d->captureDevices[x].deviceName() == MCB->CaptureDevice )
        {
            d->blnCaptureDeviceOk = true;
            d->strEntryCaptureDevice = MCB->CaptureDevice;
        }
    }
    ndx = 0;
    ndx = ui->cmbCapture->findText(MCB->CaptureDevice);
    ui->cmbCapture->setCurrentIndex(ndx);

    if ( d->blnCaptureDeviceOk )
        ui->cmbCapture->setStyleSheet("QComboBox {background-color: #ccffcc;}");
    else
        ui->cmbCapture->setStyleSheet("QComboBox {background-color: #ddaacc;}");
}

void TncSetup::gatherSettings()
{
    // make sure the MODEM CONTROL BLOCK has all the settings saved.
}

void TncSetup::connectObjects()
{
    connect(ui->OK_Button,SIGNAL(clicked(bool)),this,SLOT(slotOk(bool)));
    connect(ui->Cancel_Button,SIGNAL(clicked(bool)),this,SLOT(slotCancel(bool)));
    connect(ui->rdoTCP,SIGNAL(toggled(bool)),this,SLOT(slotTCPCheckedChg(bool)));
    connect(ui->rdoSerial,SIGNAL(toggled(bool)),this,SLOT(slotSerialCkdChg(bool)));
    connect(ui->rdoBlueTooth,SIGNAL(toggled(bool)),this,SLOT(slotBlueToothChg(bool)));
    connect(ui->cmbPlayback, SIGNAL(activated(int)),this,SLOT(slotPlayDevChg(int)));
    connect(ui->cboCapRates, SIGNAL(activated(int)),this,SLOT(slotCaptureRateChg(int)));
    connect(ui->cmbCapture, SIGNAL(activated(int)),this,SLOT(slotCaptureDevChg(int)));
    connect(ui->spinChan, SIGNAL(valueChanged(int)),this, SLOT(slotChanChg(int)));
    QMetaObject::connectSlotsByName(this);

    d->blnInitialized = true;
}

void TncSetup::readSettings()
{
    QDir dir;
#ifdef DEBUG_TNC
    qDebug() << dir.canonicalPath() + "/ardop_tnc.ini";
#endif
    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);

    settings.beginGroup("SOUND");
     MCB->CaptureDevice  = settings.value("CaptureDevice",  "default").toString();
     MCB->PlaybackDevice = settings.value("PlaybackDevice", "default").toString();
     MCB->Stereo         = settings.value("Stereo",             false).toBool();
     MCB->RxVolume       = settings.value("RxVolume",             50).toInt();
     MCB->RxAudioSampleRate = settings.value("RxSampleRate", 8000).toInt();
     MCB->TxAudioSampleRate = settings.value("TxSampleRate", 8000).toInt();
     MCB->TxDriveLevel   = settings.value("DriveLevel",         80).toInt();
     MCB->RxSquelchLevel = settings.value("Squelch",            20).toInt();
//     MCB->CaptureClock   = settings.value("CaptureClock",       0).toInt();
//     MCB->PlaybackClock  = settings.value("PlaybackClock",      0).toInt();
     d->intChannels      = settings.value("RxChannels",          1).toInt();
    settings.endGroup();

    settings.beginGroup("NETWORK");
      MCB->TCPPort        = settings.value("TCPIPPort",         8515).toInt();
      MCB->TCPAddress     = settings.value("TCPIPAddress",      "127.0.0.1").toString();
    settings.endGroup();

    settings.beginGroup("SERIAL");
      MCB->SerBaud        = settings.value("SerialBaud",         19200).toInt();
      MCB->SerCOMPort     = settings.value("SerialCOMPort",      "COM3").toString();
    settings.endGroup();

    settings.beginGroup("BLUETOOTH");
      MCB->HostPairing    = settings.value("HostPairing", "").toString();
    settings.endGroup();

    settings.beginGroup("STATION");
      MCB->Callsign        = settings.value("Callsign",         "unknown").toString();
      MCB->Password        = settings.value("LoginPassword",    "").toBool();
      MCB->SecureHostLogin = settings.value("SecureHostLogin",  false).toBool();
    settings.endGroup();

    settings.beginGroup("TNC");
      MCB->HostTCPIP      = settings.value("HostTCPIP",          true).toBool();
      MCB->HostSerial     = settings.value("HostSerial",         false).toBool();
      MCB->HostBlueTooth  = settings.value("HostBlueTooth",      false).toBool();
      MCB->CWID           = settings.value("EnableCWID",       false).toBool();
      MCB->StartMinimized = settings.value("StartMinimized",     false).toBool();
      MCB->DebugLog       = settings.value("DebugLog",           false).toBool();
      MCB->CommandTrace   = settings.value("CommandTrace",       false).toBool();
      MCB->AccumulateStats = settings.value("AccumStats",        false).toBool();
      MCB->DisplayWaterfall = settings.value("DispWaterfall",    true).toBool();
      MCB->DisplaySpectrum  = settings.value("DispSpectrum",     false).toBool();
      MCB->TuningRange    = settings.value("TuningRange",        100).toInt();
     //RCB->RadioControl   = settings.value("Enbl Radio Cont",    false).toBool();
    settings.endGroup();

    settings.beginGroup("ARDOP");
      MCB->LeaderLength   = settings.value("LeaderLenth",        0).toInt();
      MCB->TrailerLength  = settings.value("TrailerLength",      0).toInt();
      MCB->FECRepeats     = settings.value("FECRepeats",         1).toInt();
      MCB->FECMode        = settings.value("FECMode",            "").toString();
      MCB->FECId          = settings.value("FECId",              false).toBool();
      MCB->ARQConReqRepeats = settings.value("ARQConRepReq",     5).toInt();
      MCB->ProtocolMode   = settings.value("ProtocolMode",       "").toString();
      MCB->ARQTimeout     = settings.value("ARQTimeout",         100).toInt();
      MCB->SlowCPU        = settings.value("SlowCPU",            false).toBool();
      d->appver           = settings.value("Version", QApplication::applicationVersion()).toString();
    settings.endGroup();
}

void TncSetup::saveSettings()
{
#ifdef DEBUG_SETUP
    QDir dir;
    qDebug() << dir.canonicalPath() + "/ardop_tnc.ini";
#endif
    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);

    settings.beginGroup("STATION");
      settings.setValue("Callsign", MCB->Callsign);
      settings.setValue("LoginPassword", MCB->Password);
      settings.setValue("EnableCWID", MCB->CWID);
      settings.setValue("SecureHostLogin", MCB->SecureHostLogin);
    settings.endGroup();

    settings.beginGroup("NETWORK");
      settings.setValue("TCPIPPort", MCB->TCPPort);
      settings.setValue("TCPIPAddress", MCB->TCPAddress);
    settings.endGroup();

    settings.beginGroup("SERIAL");
      settings.setValue("SerialBaud",     MCB->SerBaud);
      settings.setValue("SerialCOMPort",  MCB->SerCOMPort);
    settings.endGroup();

    settings.beginGroup("BLUETOOTH");
      settings.setValue("HostPairing",    MCB->HostPairing);
    settings.endGroup();

    settings.beginGroup("SOUND");
     settings.setValue("CaptureDevice",  MCB->CaptureDevice);
     settings.setValue("PlaybackDevice", MCB->PlaybackDevice);
     settings.setValue("Stereo",         MCB->Stereo);
     settings.setValue("RxChannels",     d->intChannels);
     settings.setValue("RxVolume",       MCB->RxVolume);
     settings.setValue("RxSampleRate",   MCB->RxAudioSampleRate);
     settings.setValue("TxSampleRate",   MCB->TxAudioSampleRate);
     settings.setValue("DriveLevel",     MCB->TxDriveLevel);
     settings.setValue("Squelch",        MCB->RxSquelchLevel);
     settings.setValue("CaptureClock",   MCB->CaptureClock);
     settings.setValue("PlaybackClock",  MCB->PlaybackClock);
    settings.endGroup();

    settings.beginGroup("ARDOP");
     settings.setValue("FECRepeats",     MCB->FECRepeats);
     settings.setValue("FECMode",        MCB->FECMode);
     settings.setValue("FECId",          MCB->FECId);
     settings.setValue("ARQConRepReq",   MCB->ARQConReqRepeats);
     settings.setValue("ProtocolMode",   MCB->ProtocolMode);
     settings.setValue("ARQTimeout",     MCB->ARQTimeout);
     settings.setValue("SlowCPU",        MCB->SlowCPU);
     settings.setValue("Version",        QApplication::applicationVersion());
    settings.endGroup();

    settings.beginGroup("TNC");
     settings.setValue("HostTCPIP",      MCB->HostTCPIP);
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
     //settings.setValue("EnblRadioCont",  RCB->RadioControl);
    settings.endGroup();

    settings.sync();
}


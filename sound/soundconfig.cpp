#include <math.h>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "utils.hpp"

#include "soundconfig.hpp"

soundConfig::soundConfig(QWidget *parent) :  QDialog(parent)
{
    ui = new Ui::soundConfig;
    ui->setupUi(this);

    getCardList(inputPCMList, outputPCMList);
    ui->inputPCMNameComboBox->addItems(inputPCMList);
    ui->outputPCMNameComboBox->addItems(outputPCMList);
}


soundConfig::~soundConfig()
{
  delete ui;
}

void soundConfig::setParams()
{
    ui->inputClockLineEdit->setText(QString("%1").arg(rxClock));
    ui->outputClockLineEdit->setText(QString("%1").arg(txClock));
    quint16 x = ui->inputPCMNameComboBox->findText(inputAudioDevice,Qt::MatchCaseSensitive);
    if ( x > 0 )
        ui->inputPCMNameComboBox->setCurrentIndex(x);
    else
        ui->inputPCMNameComboBox->setCurrentIndex(0);

    x = ui->outputPCMNameComboBox->findText(outputAudioDevice,Qt::MatchCaseSensitive);
    if ( x > 0 )
        ui->outputPCMNameComboBox->setCurrentIndex(x);
    else
        ui->outputPCMNameComboBox->setCurrentIndex(0);
    ui->alsaRadioButton->setChecked(alsaSelected);
    ui->pulseRadioButton->setChecked(pulseSelected);
    ui->swapChannelCheckBox->setChecked(swapChannel);
    ui->pttToneCheckBox->setChecked(pttToneOtherChannel);
    if ( soundRoutingInput == SNDINCARD )
        ui->inFromCard->setChecked(true);
    else if ( soundRoutingInput == SNDINFROMFILE )
        ui->inFromFile->setChecked(true);
    else
        ui->inRecordFromCard->setChecked(true);

    if ( soundRoutingOutput == SNDOUTCARD )
        ui->outToCard->setChecked(true);
    else
        ui->outRecord->setChecked(true);
    ui->mbSpinBox->setValue(recordingSize);
}

void soundConfig::getParams()
{
    QString inputAudioDeviceCopy   = inputAudioDevice;
    QString  outputAudioDeviceCopy = outputAudioDevice;
    bool alsaSelectedCopy = alsaSelected;

    soundBase::edataSrc soundRoutingInputCopy  = soundRoutingInput;
    soundBase::edataDst soundRoutingOutputCopy = soundRoutingOutput;

    rxClock = ui->inputClockLineEdit->text().toDouble();
    txClock = ui->inputClockLineEdit->text().toDouble();
    inputAudioDevice = ui->inputPCMNameComboBox->currentText().trimmed();
    outputAudioDevice = ui->outputPCMNameComboBox->currentText().trimmed();
    alsaSelected  = ui->alsaRadioButton->isChecked();
    pulseSelected = ui->pulseRadioButton->isChecked();
    swapChannel   = ui->swapChannelCheckBox->isChecked();
    pttToneOtherChannel = ui->pttToneCheckBox->isChecked();

    if ( ui->inFromCard->isChecked() )
        soundBase::soundRoutingInput = soundBase::SNDINCARD;
    else if ( ui->inFromFile->isChecked() )
        soundBase::soundRoutingInput = soundBase::SNDINFROMFILE;
    else
        soundBase::soundRoutingInput = soundBase::SNDINCARDTOFILE;

    if ( ui->outToCard->isChecked() )
        soundBase::soundRoutingOutput = soundBase::SNDOUTCARD;
    else
        soundBase::soundRoutingOutput = soundBase::SNDOUTTOFILE;
    recordingSize = ui->mbSpinBox->value();
    changed = false;
    if (
           inputAudioDeviceCopy   != inputAudioDevice
        || outputAudioDeviceCopy  != outputAudioDevice
        || soundRoutingInputCopy  != soundBase::soundRoutingInput
        || soundRoutingOutputCopy != soundBase::soundRoutingOutput
        || alsaSelectedCopy       != alsaSelected
        )
    {
        changed = true;
    }
}

void soundConfig::readSettings()
{
    QDir dir;
    QString path = QDir::fromNativeSeparators(qApp->applicationDirPath() + "/ardop_tnc.ini");
    bool ok = dir.exists(path);
    if ( !ok ) saveSettings();

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("SOUND");
     rxClock = settings.value("rxclock",BASESAMPLERATE).toDouble();
     txClock = settings.value("txclock",BASESAMPLERATE).toDouble();
     if ( fabs(1 - rxClock / BASESAMPLERATE) > 0.002 )
         rxClock = BASESAMPLERATE;
     if ( fabs(1 - txClock / BASESAMPLERATE) > 0.002)
         txClock = BASESAMPLERATE;
     samplingrate = BASESAMPLERATE;
     inputAudioDevice  = settings.value("inputAudioDevice","default").toString();
     outputAudioDevice = settings.value("outputAudioDevice","default").toString();
     alsaSelected      = settings.value("alsaSelected",false).toBool();
     pulseSelected     = settings.value("pulseSelected",false).toBool();
     swapChannel       = settings.value("swapChannel",false).toBool();
     pttToneOtherChannel = settings.value("pttToneOtherChannel",false).toBool();
     soundRoutingInput  = (soundBase::edataSrc)settings.value("soundRoutingInput",  0 ).toInt();
     soundRoutingOutput = (soundBase::edataDst)settings.value("soundRoutingOutput", 0 ).toInt();
     recordingSize      = settings.value("recordingSize", 100 ).toInt();
    settings.endGroup();
    setParams();
}

void soundConfig::saveSettings()
{
    QDir dir;
    QString path = QDir::fromNativeSeparators(qApp->applicationDirPath() + "/ardop_tnc.ini");
    QSettings settings(path, QSettings::IniFormat);

    getParams();

    settings.beginGroup("SOUND");
     settings.setValue("rxclock",rxClock);
     settings.setValue("txclock",txClock);
     settings.setValue("inputAudioDevice",inputAudioDevice);
     settings.setValue("outputAudioDevice",outputAudioDevice);
     settings.setValue("alsaSelected",alsaSelected);
     settings.setValue("pulseSelected",pulseSelected);
     settings.setValue("swapChannel",swapChannel);
     settings.setValue("pttToneOtherChannel",pttToneOtherChannel);
     settings.setValue("soundRoutingInput", soundBase::soundRoutingInput );
     settings.setValue("soundRoutingOutput",soundBase::soundRoutingOutput );
     settings.setValue("recordingSize",recordingSize );
    settings.endGroup();
}


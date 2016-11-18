#include <QtCore/QSettings>
#include <QtCore/QDir>
#include "utils.hpp"

#include "wfdisplay.hpp"

wfDisplay::wfDisplay(QWidget *parent) : QWidget(parent), fftDisplay(parent)
{
    ui = new Ui::wfDisplay;
    ui->setupUi(this);
    m_disptype = Waterfall;  // default to waterfall for now

    readSettings();
    displaySettings();
}

wfDisplay::~wfDisplay()
{
    saveSettings();
    delete ui;
}

void wfDisplay::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void wfDisplay::readSettings()
{
    QDir dir;
    QString path = QDir::fromNativeSeparators(qApp->applicationDirPath() + "/ardop_tnc.ini");
    bool ok = dir.exists(path);
    if ( !ok ) saveSettings();

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("SPECTRUM");
     maxdb = settings.value("maxdb",-25).toInt();
     range = settings.value("range",35).toInt();
     avg   = settings.value("avg",0.90).toDouble();
    settings.endGroup();
}


void wfDisplay::saveSettings()
{
    QDir dir;
    QString path = QDir::fromNativeSeparators(qApp->applicationDirPath() + "/ardop_tnc.ini");
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("SPECTRUM");
     settings.setValue( "maxdb",maxdb);
     settings.setValue( "range",range);
     settings.setValue("avg",avg);
    settings.endGroup();
}


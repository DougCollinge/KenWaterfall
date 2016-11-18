#include <QtCore/QSettings>

#include "spectrumwidget.hpp"

spectrumWidget::spectrumWidget(QWidget *parent) : QFrame(parent)
{
    ui = new Ui::spectrumWidget;
    ui->setupUi(this);

    readSettings();

    connect(ui->maxDbSpinbox,SIGNAL(valueChanged(int)),SLOT(slotMaxDbChanged(int)));
    connect(ui->rangeSpinbox,SIGNAL(valueChanged(int)),SLOT(slotRangeChanged(int)));
    connect(ui->avgDoubleSpinBox,SIGNAL(valueChanged(double)),SLOT(slotAvgChanged(double)));
}

spectrumWidget::~spectrumWidget()
{
    writeSettings();
    delete ui;
}

void spectrumWidget::setMCB(ModemControlBlock *mcb)
{
    MCB = mcb;
    ui->spectrWidget->setMCB(MCB);
}

void spectrumWidget::realFFT(QByteArray *iBuffer)
{
    for ( int x = 0; x < iBuffer->length(); ++x )
    {
        double dx = iBuffer[x].toDouble();
        fftFunc.realFFT(&dx);
        ui->spectrWidget->plotFFT(fftFunc.out);
        ui->waterfallWidget->plotFFT(fftFunc.out);
    }
}

void spectrumWidget::init(int length,int slices,int isamplingrate)
{
    fftFunc.init(length,slices,isamplingrate);
    ui->spectrWidget->init(length,slices,isamplingrate);
    ui->waterfallWidget->init(length,slices,isamplingrate);
}


void spectrumWidget::readSettings()
{
    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("SPECTRUM");
    maxdb = settings.value("maxdb",-25).toInt();
    range = settings.value("range",35).toInt();
    avg   = settings.value("avg",0.90).toDouble();
    settings.endGroup();
    setParams();
}


void spectrumWidget::writeSettings()
{
    getParams();

    QSettings settings("ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("SPECTRUM");
      settings.setValue("maxdb",maxdb);
      settings.setValue("range",range);
      settings.setValue("avg",  avg);
    settings.endGroup();
    settings.sync();
}

void spectrumWidget::getParams()
{
    ui->maxDbSpinbox->setValue(maxdb);
    ui->rangeSpinbox->setValue(range);
    ui->avgDoubleSpinBox->setValue(avg);
}

void spectrumWidget::setParams()
{
    ui->maxDbSpinbox->setValue(maxdb);
    ui->rangeSpinbox->setValue(range);
    ui->avgDoubleSpinBox->setValue(avg);
    slotMaxDbChanged(maxdb);
    slotRangeChanged(range);
    slotAvgChanged(avg);

}

void spectrumWidget::displaySettings(bool drm)
{
    ui->spectrWidget->displayWaterfall(false);
    ui->waterfallWidget->displayWaterfall(true);
    if(drm)
    {
        ui->spectrWidget->setMarkers(725,1475,1850);
        ui->waterfallWidget->setMarkers(725,1475,1850);
        ui->markerLabelSpectrum->setMarkers(725,1475,1850);
        ui->markerLabelWF->setMarkers(725,1475,1850);

    }
    else
    {
        ui->spectrWidget->setMarkers(1200,1500,2300);
        ui->waterfallWidget->setMarkers(1200,1500,2300);
        ui->markerLabelSpectrum->setMarkers(1200,1500,2300);
        ui->markerLabelWF->setMarkers(1200,1500,2300);
    }
}

void spectrumWidget::slotMaxDbChanged(int mb)
{
  ui->spectrWidget->setMaxDb(mb);
  ui->waterfallWidget->setMaxDb(mb);
  maxdb=mb;
}
void spectrumWidget::slotRangeChanged(int rg)
{
  ui->spectrWidget->setRange(rg);
  ui->waterfallWidget->setRange(rg);
  range=rg;
}

void spectrumWidget::slotAvgChanged(double d)
{
   ui->spectrWidget->setAvg(d);
   ui->waterfallWidget->setAvg(d);
}



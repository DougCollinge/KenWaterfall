#ifndef SPECTRUMWIDGET_HPP
#define SPECTRUMWIDGET_HPP
#include "fftw3wrapper.hpp"

#include <QtWidgets/QFrame>

#include "ui_spectrumwidget.h"

namespace Ui {
  class spectrumWidget;
  }

class spectrumWidget : public QFrame
{
    Q_OBJECT

public:
    spectrumWidget(QWidget *parent = 0);
    ~spectrumWidget();

    void init(qint32 length, qint32 slices, qint32 isamplingrate);
    //  void realFFT(short int *iBuffer);
    //  void realFFT(float *iBuffer);
    void readSettings();
    void writeSettings();
    void displaySettings(bool drm);
    void setMCB(ModemControlBlock *mcb);
    double getVolumeDb();

public slots:
    void realFFT(QByteArray *iBuffer);

private slots:
    void slotMaxDbChanged(qint32 mb);
    void slotRangeChanged(qint32 rg);
    void getParams();
    void setParams();
    void slotAvgChanged(double);

private:
    qint32 maxdb;
    qint32 range;
    double avg;

    fftw3calc fftFunc;

    ModemControlBlock  *MCB;
    Ui::spectrumWidget *ui;
};


#endif // SPECTRUMWIDGET_HPP

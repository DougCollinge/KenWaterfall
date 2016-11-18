#ifndef WATERFALLWIDGET_HPP
#define WATERFALLWIDGET_HPP
#include <QtCore/QMutex>
#include <QtWidgets/QLabel>
#include <QtGui/QImage>
#include <QtGui/QPolygon>

#include "fftw3.h"


#define FFTAVERAGING 0.1
#define VOLAVG 0.1
#define FFTHIGH 2900
#define FFTLOW  200
#define FFTSPAN (FFTHIGH-FFTLOW)
#define FFTMAX 6
#define RANGE 30.

class FFTDisplayWidget : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(double m_fftMax READ getMaxDb WRITE setMaxDb)
    Q_PROPERTY(double m_range  READ getRange WRITE setRange)
    Q_PROPERTY(double m_avg    READ getAvg   WRITE setAvg)

public:
    explicit FFTDisplayWidget(QWidget *parent=0);
    ~FFTDisplayWidget();

    //          PROPERTY SETS  AND GETS           //
    void setMaxDb(double mb)           { m_fftMax = mb; }
    void setRange(double rg)           { m_range  = rg; }
    void setAvg(double d)              { m_avg = d;  }

    double getMaxDb()                  { return m_fftMax; }
    double getRange()                  { return m_range;  }
    double getAvg()                    { return m_avg;    }
    //                                           //

    // ---------------------------------------------------
    //   PUBLIC METHODS AND FUNCTIONS
    // ---------------------------------------------------
    void init(quint32 length, quint32 slices, quint64 isamplingrate);
    void plotFFT(double *fftData);
    void displayWaterfall(bool wf);
    void setMarkers(quint32 mrk1=0, quint32 mrk2=0, quint32 mrk3=0);
    QImage *getImage();

protected:
    void paintEvent(QPaintEvent *p);

private:
    // property local vars  //
    double m_fftMax;
    double m_range;
    double m_avg;
    //                      //

    class WFPrivateData;
    WFPrivateData *wfd;

    void drawMarkers(QPainter *painter, int top, int bot);
};

#endif // WATERFALLWIDGET_HPP

#ifndef FFTDISPLAY_HPP
#define FFTDISPLAY_HPP
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtGui/QPolygon>
#include <QtWidgets/QLabel>

#include "ardop_global.hpp"
#include "fftw3wrapper.hpp"

#define VOLAVG 0.1

#define FFTAVERAGING 0.1
#define FFTHIGH 3000
#define FFTLOW  200
#define FFTSPAN (FFTHIGH-FFTLOW)
#define FFTMAX 6
#define RANGE 30.0

class fftDisplay : public QLabel
{
    Q_OBJECT
    // additional properties to QLabel
    Q_PROPERTY(qint32 m_maxdb READ getMaxDb WRITE setMaxDb)
    Q_PROPERTY(qint32 m_range READ getRange WRITE setRange)
    Q_PROPERTY(bool   m_show  READ isShown  WRITE setShow)

public:
    fftDisplay(QWidget *parent=0);
    ~fftDisplay();

    //  PROPERTY SETS AND GETS   //
    void setMaxDb(qint32 mb)                    { m_maxdb = mb; }
    void setRange(qint32 rg)                    { m_range = rg; }
    void setShow(bool wf)                       { m_show  = wf; }

    qint32 getMaxDb()                           { return m_maxdb; }
    qint32 getRange()                           { return m_range; }
    bool   isShown()                            { return m_show;  }
    //                           //

    void init(qint32 length,
              qint32 slices,
              qint32 isamplingrate);
    void setMarkers(qint32 mrk1 = 0,
                    qint32 mrk2 = 0,
                    qint32 mrk3 = 0);
    void setAvg(double d)                       { avgVal  = d;  }
    void setMCB(ModemControlBlock *mcb)         { MCB = mcb;    }

    void plotFFT(double *fftData);
    void displayWaterfall(bool wf)              { showWaterfall = wf; }

protected:
    void paintEvent(QPaintEvent *p);
    //  void mousePressEvent( QMouseEvent *e );

private:
    //   property vars   //
    bool   m_show;
    double m_maxdb;  // this is the FFTMAX value from the defined values
    double m_range;
    // ---------------- //

    //  bool ready;
    bool showWaterfall;

    qint32 windowSize;
    qint32 fftLength;
    qint32 samplingrate;
    qint32 marker1;
    qint32 marker2;
    qint32 marker3;
    qint32 binBegin;
    qint32 binEnd;
    qint32 binDiff;
    qint32 imWidth;
    qint32 imHeight;
    qint32 displayCounter;

    double maxMagnitude;
    double step;  //freq step per bin
    double avgVal;


    QImage scaledImage;

    QMutex mutex;

    fftw3calc fftcalc;

    double*  arMagSAvg;
    double*  arMagWAvg;

    QPolygon *fftArray;

    QImage   *imagePtr;

    ModemControlBlock *MCB;
};


#endif // FFTDISPLAY_HPP

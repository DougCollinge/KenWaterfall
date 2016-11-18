#include <math.h>

#include <QtCore/QDebug>
#include <QtCore/QSize>
#include <QtCore/QVector>
#include <QtGui/QPainter>

#include "fftdisplay.hpp"

fftDisplay::fftDisplay(QWidget *parent) : QLabel(parent)
{
    m_show    = true;
    m_maxdb   = FFTMAX;
    m_range   = RANGE;
    avgVal    = 0.05;

    setScaledContents(true);

    imWidth   = -1;
    imHeight  = -1;
    displayCounter = 0;
}

fftDisplay::~fftDisplay()
{
    delete imagePtr;
    if ( fftArray )  delete fftArray;
    if ( arMagSAvg ) delete []arMagSAvg;
    if ( arMagWAvg ) delete []arMagWAvg;
}

void fftDisplay::paintEvent(QPaintEvent *p)
{
    QPen pn;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    if ( !m_show )
    {
        pn.setColor(Qt::red);
        pn.setWidth(1);
        painter.setPen(pn);
        painter.drawLine((((marker1-FFTLOW) * imWidth) / FFTSPAN),0,(((marker1 - FFTLOW) * imWidth) / FFTSPAN),imHeight);
        painter.drawLine((((marker2-FFTLOW) * imWidth) / FFTSPAN),0,(((marker2 - FFTLOW) * imWidth) / FFTSPAN),imHeight);
        painter.drawLine((((marker3-FFTLOW) * imWidth) / FFTSPAN),0,(((marker3 - FFTLOW) * imWidth) / FFTSPAN),imHeight);
        pn.setColor(Qt::green);
        painter.setPen(pn);
        painter.drawPolyline(*fftArray);
    }
    else
    {
        if ( imagePtr )
        {
            scaledImage = imagePtr->scaled(QSize(width(),height()));
            painter.drawImage(0,0,scaledImage);
        }
    }
    QLabel::paintEvent(p);
}


void fftDisplay::setMarkers(qint32 mrk1, qint32 mrk2, qint32 mrk3)
{
    marker1=mrk1;
    marker2=mrk2;
    marker3=mrk3;
    update();
}

void fftDisplay::plotFFT(double *fftData)
{
    qint32 i;
    qint32 j;
    double re;
    double imag;
    double tmp;
    QColor c;

    if ( (!m_show) && ( MCB->SlowCPU ) )
    {
        if ( ++displayCounter < 1 )
            return;
        else
            displayCounter = 0;
    }

    if ( (imWidth != width()) || (imHeight != height()) )
    {
        if ( imWidth != width() )
        {
            if ( arMagWAvg != NULL ) delete arMagWAvg;
            // resize the array
            size_t newSize = width() * 2;
            double* newArr = new double[newSize];

            size_t size = newSize;
            delete [] arMagWAvg;

            memcpy( newArr, arMagWAvg, size * sizeof(double) );

            arMagWAvg = newArr;
            // set all elements to zero
            for ( i = 0; i < width(); ++i )
            {
                arMagWAvg[i] = 0;
            }
        }
        imWidth  = width();
        imHeight = height();

        if ( m_show )
        {
            if ( imagePtr == NULL )
            {
                imagePtr=new QImage( width(),height(),QImage::Format_RGB32);
                imagePtr->fill(Qt::black);
            }
            else
            {

                *imagePtr=imagePtr->scaled(QSize(imWidth,imHeight));
            }
        }
    }

    if ( !m_show )
    {
        for ( i = binBegin, j = 0; i < binEnd; ++i,++j )
        {
            re   = fftData[i] / fftLength;
            imag = fftData[fftLength-i] / fftLength;
            tmp  = 10 * log10((re * re + imag * imag)) - 77.27;  // 0.5 Vtt is 0db
            if ( arMagSAvg[j] < -100 )  arMagSAvg[j] = -100;
            if ( arMagSAvg[j] < tmp )
                arMagSAvg[j] = arMagSAvg[j] * (1 - 0.4) + 0.4 * tmp;
            else
                arMagSAvg[j] = arMagSAvg[j] * (1 - avgVal) + avgVal * tmp;
            tmp = (m_maxdb - arMagSAvg[j]) / m_range;
            if ( tmp < 0 ) tmp = 0;
            if ( tmp > 1 ) tmp = 1;
            int pos = (int)rint((double)(j * (imWidth-1)) / (double)binDiff);
            fftArray->setPoint(j,pos,(imHeight - 1) * tmp); // range 0 -> -1
        }
    }
    else
    {
        memmove(imagePtr->scanLine(1),imagePtr->scanLine(0),(imWidth * (imHeight - 2)) * sizeof(uint));
        quint32 *ptr = (quint32 *)imagePtr->scanLine(0);
        memmove(imagePtr->scanLine(1),imagePtr->scanLine(0),(imWidth*(imHeight-2))*sizeof(uint));
        ptr = (quint32 *)imagePtr->scanLine(0);
        for( i = 0; i < imWidth; ++i )
        {
            qint32 idx = rint((double)(FFTLOW + (i * FFTSPAN) / (double)imWidth) / step);
            re   = fftData[idx] / fftLength;
            imag = fftData[fftLength - idx] / fftLength;
            tmp  = 10*log10((re * re + imag * imag)) - 77.27;  // 0.5 Vtt is 0db
            arMagWAvg[i] = arMagWAvg[i] * (1 - avgVal) + avgVal * tmp;
            if ( arMagWAvg[i] < -100 )
            {
                arMagWAvg[i] = -100;
            }
            tmp = 1 - (m_maxdb - arMagWAvg[i]) / m_range;
            if ( tmp < 0 ) tmp = 0;
            if ( tmp > 1 ) tmp = 1;
            c.setHsv(240 - tmp * 60,255,tmp * 255);
            ptr[i] = c.rgb();
        }
    }
    update();
}


void fftDisplay::init(qint32 length,qint32 nblocks,qint32 isamplingrate)
{
#ifdef DEBUG_FFT
    qDebug() << "Initializing fftDisplay";
#endif

    qint32 i;

    windowSize   = length;
    fftLength    = windowSize * nblocks;
    samplingrate = isamplingrate;
//    if ( fftArray )  delete fftArray;
//    if ( arMagSAvg ) delete arMagSAvg;
    step = (double)samplingrate / (double)fftLength;  //freq step per bin
    binBegin  = (int)rint(FFTLOW / step);
    binEnd    = (int)rint(FFTHIGH / step);
    binDiff   = binEnd - binBegin;
#ifdef DEBUG_FFT
    qDebug() << "Window Sz:" << QString("%1").arg(windowSize) << "\n"
             << "  FFT len:" << QString("%1").arg(fftLength)  << "\n"
             << "Smpl Rate:" << QString("%1").arg(samplingrate)  << "\n"
             << "     Step:" << QString("%1").arg(step) << "\n"
             << "   St Bin:" << QString("%1").arg(binBegin) << "\n"
             << "  End Bin:" << QString("%1").arg(binEnd) << "\n"
             << "     Diff:" << QString("%1").arg(binDiff) << "\n";
#endif

    fftArray  = new QPolygon(binDiff);
    arMagSAvg = new double[binDiff];

    for( i = 0; i < binDiff; ++i )
    {
        arMagSAvg[i] = -30.0;
    }

#ifdef DEBUG_FFT
    qDebug() << "Updating Display";
#endif
    update();
    QLabel::update();
}


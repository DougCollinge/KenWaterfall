#include <math.h>

#include <QtGui/QPainter>

#include "utils.hpp"

#include "fftw3wrapper.hpp"
#include "vumeterwidget.hpp"
#include "waterfallwidget.hpp"

class FFTDisplayWidget::WFPrivateData
{
public:
    bool showWaterfall;
    bool slowCPU;

    quint32 fftLength;
    quint32 marker1;
    quint32 marker2;
    quint32 marker3;
    quint32 windowSize;

    quint64 samplingrate;

    int binBegin;
    int binEnd;
    int binDiff;
    int imWidth;
    int imHeight;
    int displayCounter;

    double maxMagnitude;
    double step;  //freq step per bin

    QImage    scaledImage;

    QMutex    mutex;

    double   *arMagSAvg;
    double   *arMagWAvg;

    QImage   *imagePtr;
    QPolygon *fftArray;
};

FFTDisplayWidget::FFTDisplayWidget(QWidget *parent) : QLabel(parent)
{
    wfd = new WFPrivateData;

    // default values for property vars
    m_fftMax       = FFTMAX;
    m_range        = RANGE;
    m_avg          = 0.05;
    // init local vars
    wfd->slowCPU        = false;
    wfd->showWaterfall  = false;
    wfd->arMagSAvg      = NULL;
    wfd->fftArray       = new QPolygon(4096);
    wfd->imagePtr       = NULL;
    wfd->arMagWAvg      = NULL;
    wfd->imWidth        = -1;
    wfd->imHeight       = -1;
    wfd->displayCounter = 0;

    setScaledContents(true);

}

FFTDisplayWidget::~FFTDisplayWidget()
{
    delete wfd->imagePtr;

    if ( wfd->fftArray )  delete wfd->fftArray;
    if ( wfd->arMagSAvg ) delete wfd->arMagSAvg;
    if ( wfd->arMagWAvg ) delete wfd->arMagWAvg;
}


void FFTDisplayWidget::paintEvent(QPaintEvent *p)
{
    QPen pn;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    if ( !wfd->showWaterfall )
    {
        pn.setColor(Qt::red);
        pn.setWidth(1);
        painter.setPen(pn);
        drawMarkers(&painter,0,wfd->imHeight);
        pn.setColor(Qt::green);
        painter.setPen(pn);
        painter.drawPolyline(*wfd->fftArray);
    }
    else
    {
        if( wfd->imagePtr )
        {
            wfd->scaledImage = wfd->imagePtr->scaled(QSize(width(),height()));
            painter.drawImage(0,0,wfd->scaledImage);
        }
    }
    QLabel::paintEvent(p);
}

void FFTDisplayWidget::displayWaterfall(bool wf)
{
    wfd->showWaterfall = wf;
}

void FFTDisplayWidget::init(quint32 length, quint32 nblocks, quint64 isamplingrate)
{
    qint32 i;

    wfd->windowSize   = length;
    wfd->fftLength    = wfd->windowSize * nblocks;
    wfd->samplingrate = isamplingrate;

    if ( wfd->fftArray )  delete wfd->fftArray;
    if ( wfd->arMagSAvg ) delete wfd->arMagSAvg;
    wfd->step = (double)wfd->samplingrate / (double)wfd->fftLength;  //freq step per bin
    wfd->binBegin = (int)rint(FFTLOW / wfd->step);
    wfd->binEnd   = (int) rint(FFTHIGH / wfd->step);
    wfd->binDiff  = wfd->binEnd - wfd->binBegin;

    wfd->fftArray  = new QPolygon(wfd->binDiff);
    wfd->arMagSAvg = new double[wfd->binDiff];

    for ( i = 0; i < wfd->binDiff; ++i )
    {
        wfd->arMagSAvg[i] = -30.0;
    }

    // create the fftw plan
    //  plan = fftw_plan_r2r_1d(fftLength, dataBuffer, out, FFTW_R2HC, FFTW_ESTIMATE);
    update();
    QLabel::update();
}




void FFTDisplayWidget::setMarkers(quint32 mrk1, quint32 mrk2, quint32 mrk3)
{
    wfd->marker1=mrk1;
    wfd->marker2=mrk2;
    wfd->marker3=mrk3;
    update();
}

void FFTDisplayWidget::plotFFT(double *fftData)
{
    quint32 i;
    quint32 j;
    double re;
    double imag;
    double tmp;
    QColor  c;

    if ( !wfd->showWaterfall && wfd->slowCPU )
    {
        if ( (++wfd->displayCounter) < 1 )
            return;
        else
            wfd->displayCounter = 0;
    }

    if ( (wfd->imWidth != width()) || (wfd->imHeight != height()) )
    {
        if ( wfd->imWidth != width() )
        {
            if ( wfd->arMagWAvg != NULL) delete wfd->arMagWAvg;
            wfd->arMagWAvg = new double[width()];
            for ( i = 0; i < width(); ++i )
            {
                wfd->arMagWAvg[i] = 0;
            }
        }
        wfd->imWidth  = width();
        wfd->imHeight = height();
        if ( wfd->showWaterfall )
        {
            if ( wfd->imagePtr == NULL )
            {
                wfd->imagePtr = new QImage( width(),height(),QImage::Format_RGB32);
                wfd->imagePtr->fill(Qt::black);
            }
            else
            {
                *wfd->imagePtr = wfd->imagePtr->scaled(QSize(wfd->imWidth,wfd->imHeight));
            }
        }
    }

    if ( !wfd->showWaterfall )
    {
        for ( i = wfd->binBegin, j = 0; i < wfd->binEnd; ++i,++j )
        {
            re   = fftData[i] / wfd->fftLength;
            imag = fftData[wfd->fftLength-i] / wfd->fftLength;
            tmp  = 10*log10((re * re + imag * imag)) - 77.27;  // 0.5 Vtt is 0db
            if ( wfd->arMagSAvg[j] < -100 )
            {
                wfd->arMagSAvg[j] = -100;
            }
            if ( wfd->arMagSAvg[j] < tmp )
                wfd->arMagSAvg[j] = wfd->arMagSAvg[j] * (1 - 0.4) + 0.4 * tmp;
            else
                wfd->arMagSAvg[j] = wfd->arMagSAvg[j] * (1 - m_avg) + m_avg * tmp;
            tmp = (m_fftMax - wfd->arMagSAvg[j]) / m_range;
            if ( tmp < 0 ) tmp = 0;
            if ( tmp > 1 ) tmp = 1;
            int pos = (int)rint((double)(j * (wfd->imWidth - 1)) / (double)wfd->binDiff);
            wfd->fftArray->setPoint(j,pos,(wfd->imHeight - 1) * tmp); // range 0 -> -1
        }
    }
    else
    {
        memmove(wfd->imagePtr->scanLine(1),wfd->imagePtr->scanLine(0),(wfd->imWidth * (wfd->imHeight - 2)) * sizeof(quint32));
        quint32 *ptr=(quint32 *)wfd->imagePtr->scanLine(0);
        for ( i = 0; i < wfd->imWidth; ++i )
        {
            qint32 idx = rint((double)(FFTLOW + (i * FFTSPAN) / (double)wfd->imWidth) / wfd->step);
            re   = fftData[idx] / wfd->fftLength;
            imag = fftData[wfd->fftLength-idx] / wfd->fftLength;
            tmp  = 10*log10((re * re + imag * imag)) - 77.27;  // 0.5 Vtt is 0db
            wfd->arMagWAvg[i] = wfd->arMagWAvg[i] * (1 - m_avg) + m_avg * tmp;
            if ( wfd->arMagWAvg[i] < -100 )
            {
                wfd->arMagWAvg[i] = -100;
            }
            tmp = 1 - (m_fftMax - wfd->arMagWAvg[i]) / m_range;
            if ( tmp < 0 ) tmp = 0;
            if ( tmp > 1 ) tmp = 1;
            c.setHsv(240 - tmp * 60, 255, tmp * 255);
            ptr[i] = c.rgb();
        }
    }
    update();
}


void FFTDisplayWidget::drawMarkers(QPainter *painter, int top, int bot)
{
    painter->drawLine((((wfd->marker1 - FFTLOW) * wfd->imWidth) / FFTSPAN),top,(((wfd->marker1 - FFTLOW) *wfd->imWidth) / FFTSPAN),bot);
    painter->drawLine((((wfd->marker2 - FFTLOW) * wfd->imWidth) / FFTSPAN),top,(((wfd->marker2 - FFTLOW) *wfd->imWidth) / FFTSPAN),bot);
    painter->drawLine((((wfd->marker3 - FFTLOW) * wfd->imWidth) / FFTSPAN),top,(((wfd->marker3 - FFTLOW) *wfd->imWidth) / FFTSPAN),bot);
}

QImage *FFTDisplayWidget::getImage()
{
    QImage *im = new QImage(width(),height()+10,QImage::Format_RGB32);
    QPainter p(im);
    QPen pn;

    if ( !wfd->showWaterfall )
    {
        //
    }
    else
    {
        if ( wfd->imagePtr )
        {
            im->fill(Qt::black);
            p.drawImage(0,5,*wfd->imagePtr);
            pn.setColor(Qt::red);
            pn.setWidth(3);
            p.setPen(pn);
            drawMarkers(&p,0,4);
            drawMarkers(&p,height() + 5,height() + 9);
        }
    }
    return im;
}


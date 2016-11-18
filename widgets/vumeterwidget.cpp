#include <QtCore/QDebug>
#include <QtGui/QPainter>

#include "vumeterwidget.hpp"

#include <QPainter>
#include <QBrush>
#include <QStyle>
#include <QPen>
#include <QColor>

VUMeterWidget::VUMeterWidget(QWidget *parent) : QProgressBar(parent)
{
    setMinimum(0);
    setMaximum(100);
    pos70 = QStyle::sliderPositionFromValue(minimum(), maximum(), 70, width());
    pos85 = QStyle::sliderPositionFromValue(minimum(), maximum(), 85, width());
}

VUMeterWidget::~VUMeterWidget()
{

}

void VUMeterWidget::paintEvent(QPaintEvent*)
{
    int val = value();
    pos = QStyle::sliderPositionFromValue(0, 100, val, width());
//    qDebug() << "val:"   << val;
//    qDebug() << "pos:"   << pos;
//    qDebug() << "pos70:" << pos70;
//    qDebug() << "pos85:" << pos70;
//    qDebug() << "w:"     << width();

    QPainter p(this);

    if ( val >= 0 && val <= 70 )
    {
        // draw default gradient color
        QLinearGradient linearGrad1(0,0,0,23);
        linearGrad1.setColorAt(0, QColor(150,255,150));
        linearGrad1.setColorAt(0.4, QColor(200,255,200));
        linearGrad1.setColorAt(1, QColor(50, 255, 0));
        QRect rect1(this->rect().topLeft(), this->rect().bottomRight());
        p.fillRect(rect1,linearGrad1);
        p.drawRect(0,0,pos,height());
    }
    else if ( val > 70 && val <= 85 )
    {
        // draw default gradient color + warning color
        QLinearGradient linearGrad1(0,0,0,23);
        linearGrad1.setColorAt(0, QColor(150,255,150));
        linearGrad1.setColorAt(0.4, QColor(200,255,200));
        linearGrad1.setColorAt(1, QColor(50, 255, 0));
        QRect rect_linear1(this->rect().topLeft(), this->rect().bottomRight());
        p.fillRect(rect_linear1, linearGrad1);
        p.drawRect(0,0,pos,height());
        QLinearGradient linearGrad2(pos70,0,pos70,23);
        QRect rect_linear2(pos70,0,pos85, height());
        linearGrad2.setColorAt(0, QColor(255,204,0));
        linearGrad2.setColorAt(0.4, QColor(255,220,160));
        linearGrad2.setColorAt(1, QColor(204, 160, 0));
        p.fillRect(rect_linear2, linearGrad2);
        p.drawRect(pos70, 0, pos - pos70, height());
    }
    else if ( val > 85 )
    {
        if ( val > 100 ) val = 100;
        // draw default gradient color + warning color + caution
        QLinearGradient linearGrad1(0,0,0,23);
        linearGrad1.setColorAt(0, QColor(150,255,150));
        linearGrad1.setColorAt(0.4, QColor(200,255,200));
        linearGrad1.setColorAt(1, QColor(50, 255, 0));
        QRect rect_linear1(this->rect().topLeft(), this->rect().bottomRight());
        p.fillRect(rect_linear1, linearGrad1);
        p.drawRect(0,0,pos,height());
        QLinearGradient linearGrad2(pos70,0,pos70,23);
        linearGrad2.setColorAt(0, QColor(255,204,0));
        linearGrad2.setColorAt(0.4, QColor(255,220,160));
        linearGrad2.setColorAt(1, QColor(204, 160, 0));
        QRect rect_linear2(pos70,0,pos85, height());
        p.fillRect(rect_linear2, linearGrad2);
        p.drawRect(pos70, 0, pos - pos70, height());
        QLinearGradient linearGrad3(pos85,0,pos85,23);
        linearGrad3.setColorAt(0, QColor(255,0,0));
        linearGrad3.setColorAt(0.4, QColor(255,150,150));
        linearGrad3.setColorAt(1, QColor(204, 0, 0));
        QRect rect_linear3(pos85,0,pos85, height());
        p.fillRect(rect_linear3, linearGrad3);
        p.drawRect(pos70, 0, pos85 - pos70,height());
    }

    QLinearGradient linearGrad0(0,0,width(),height());
    linearGrad0.setColorAt(0, QColor(200,200,200));
    linearGrad0.setColorAt(0.4, QColor(230,230,230));
    linearGrad0.setColorAt(1, QColor(200, 200, 200));
    QRect rect_linear0(pos,0,width(), height());
    p.fillRect(rect_linear0, linearGrad0);
//    p.setPen(Qt::lightGray);
//    p.setBrush(QBrush(Qt::lightGray));
    p.drawRect(pos, 0, width(), height());


    p.setPen(Qt::black);
    p.setBrush(QBrush(Qt::black));
}


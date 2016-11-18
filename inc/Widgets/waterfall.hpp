#ifndef WATERFALL_HPP
#define WATERFALL_HPP
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QRectF>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QWidget>

#include "qwt_plot_spectrogram.h"
#include "qwt_scale_map.h"

class WaterfallWidget : public QwtPlotSpectrogram
{

public:
    WaterfallWidget(const QString &title = QString::null);
    virtual ~WaterfallWidget();

protected:
    virtual QImage renderImage(const QwtScaleMap &xMap,
                               const QwtScaleMap &yMap,
                               const QRectF &rect) const;

private:
    mutable QImage cachedImage_;
    mutable QwtScaleMap cachedXMap_;
    mutable QwtScaleMap cachedYMap_;
    mutable QRectF cachedArea_;
    mutable QRect cachedRect_;
    mutable double cachedXShift_;
    mutable double cachedYShift_;
};

#endif // WATERFALL_HPP

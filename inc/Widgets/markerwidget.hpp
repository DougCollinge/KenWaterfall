#ifndef MARKERWIDGET_HPP
#define MARKERWIDGET_HPP
#include <QtGui/QPaintEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

class markerWidget : public QLabel
{
    Q_OBJECT
public:
    explicit markerWidget(QWidget *parent = 0);

    void setMarkerLabel(QLabel *markerLabel);
    void setMarkers(qint32 mrk1, qint32 mrk2 = 0, qint32 mrk3 = 0)
    {
        marker1 = mrk1;
        marker2 = mrk2;
        marker3 = mrk3;update();
    }

signals:

public slots:

private:
    qint32 marker1;
    qint32 marker2;
    qint32 marker3;

    void paintEvent(QPaintEvent *p);
};

#endif // MARKERWIDGET_HPP

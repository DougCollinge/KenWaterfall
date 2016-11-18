#ifndef VUMETERWIDGET_HPP
#define VUMETERWIDGET_HPP
#include <QWidget>
#include <QProgressBar>
#include <QPaintEvent>
#include <QLinearGradient>

class VUMeterWidget : public QProgressBar
{
    Q_OBJECT

public:
    explicit VUMeterWidget(QWidget *parent = 0);
    ~VUMeterWidget();

protected:

    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

signals:

public slots:

private:
    int pos;
    int pos70;
    int pos85;
};

#endif // VUMETERWIDGET_HPP

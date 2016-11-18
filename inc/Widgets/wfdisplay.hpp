#ifndef FORM_HPP
#define FORM_HPP

#include <QtWidgets/QWidget>

#include "fftcalc.hpp"
#include "fftdisplay.hpp"
#include "ui_wfdisplay.h"

namespace Ui {
class wfDisplay;
}

class wfDisplay : public QWidget, public fftDisplay
{
    Q_OBJECT

public:
    explicit wfDisplay(QWidget *parent = 0);
    virtual ~wfDisplay();

    void getParams();
    void setParams();

    void init();
    void clear();
    double getVolumeDb();

protected:
    void changeEvent(QEvent *);

private slots:
    void slotDispTypeChgd();

private:
};

#endif // FORM_HPP

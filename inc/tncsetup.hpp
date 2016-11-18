#ifndef TNCSETUP_HPP
#define TNCSETUP_HPP
#include <QtCore/QObject>
#include <QtGui/QShowEvent>
#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtMultimedia/QAudio>

#include "ardop_global.hpp"
#include "ui_tncsetup.h"

namespace Ui {
class TncSetup;
}

/*! \ingroup TNCPanel
 * \class TncSetup
 * \brief The TNC Setup UI panel for settings
 * \details
 * This class implements the setup functions for the TNC. All settings
 * displayed are the configurable settings of the TNC allowable.
 * This panel is called from the setup menu on the TNCPanel. */
class TncSetup : public QDialog
{
    Q_OBJECT

public:
    explicit TncSetup(QWidget *parent = 0);
    virtual ~TncSetup();

    void setControlBlock(ModemControlBlock *mcb);

protected:
    void changeEvent(QEvent *);
    void showEvent(QShowEvent *);

private slots:
    void slotOk(bool);
    void slotCancel(bool);
    void slotTCPCheckedChg(bool);
    void slotSerialCkdChg(bool);
    void slotBlueToothChg(bool);
    void slotPlayDevChg(int);
    void slotChanChg(int);
    void slotCaptureDevChg(int);
    void slotCaptureRateChg(int);

private:
    QSerialPort *SerialPort;

    class TncPrivate;
    TncPrivate *d;

    Ui::TncSetup *ui;

    ModemControlBlock *MCB;
//    RadioControlBlock *RCB;

    void initForm();
    void connectObjects();
    void displaySettings();
    void gatherSettings();
    void readSettings();
    void saveSettings();
    bool CheckValidCallsignSyntax(QString);
};

#endif // TNCSETUP_HPP

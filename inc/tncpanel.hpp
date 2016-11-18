#ifndef TNCPANEL_HPP
#define TNCPANEL_HPP
#include <math.h>

#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtCore/QIODevice>
#include <QtCore/QVarLengthArray>
#include <QtGui/QShowEvent>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>
//#include <QtSerialPort/QSerialPort>

#include "ardop_global.hpp"
#include "audioinputio.hpp"
#include "modem.hpp"

#include "ui_tncpanel.h"

namespace Ui {
class TNCPanel;
}

class MainPrivate;
class TNCSetup;

/*! \ingroup ARDOPTNC
 * \class TNCPanel
 * \brief The TNC Display Panel and protocol logic
 * \details
 * This module is the UI graphical interface (GUI) to
 * the ARDOP protocol and modem.
 *
 * The TNC displays many pieces of information while
 * audio is being either captured or played back:
 *
 * The current RMS level of the audio stream, and the recent
 * 'high watermark' level. The state of the TNC and
 * protocol, and the demodulation of data to datascope display,
 * just to name a few.
 *
 * Spectrum analysis is performed by calculating the
 * Fast Fourier Transform (FFT) of a segment of audio data.
 * An open-source library, FFTW3, against which the application
 * is dynamically linked on Windows, Linux, and Mac which
 * is used to compute the transform data.
 *
 * \note
 * The ModemControlBlock (MCB) and the RadioContrlBlock (RCB)
 * are created and used here.  Do Not Replicate to local
 * vars except for temporary volatile use. */
class TNCPanel : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool m_soundIsPlaying READ soundIsPlaying WRITE setSoundIsPlaying)

public:
    TNCPanel();
    ~TNCPanel();

    bool blnCodecStarted;
    bool blnInTestMode;
    bool blnClosing;
    bool blnFramePending;
    bool blnLastPTT;
    bool blnRadioMenuEnabled;

    quint8 bytLastACKedDataFrameType;  //!< added 0.4.3.1 to accomodate once a data frame was AKEed to return ACK even if failed decode  RM 2/28/2016

    // PROPERTY SETS AND GETS       //
    void setSoundIsPlaying(bool p)           { m_soundIsPlaying = p; }

    bool soundIsPlaying()                    { return m_soundIsPlaying; }
    //                              //

    Ui::TNCPanel *ui;

    // ------------------------------------
    // Data VARS
    // ------------------------------------
    qint32 intRepeatCnt;

    double dblMaxLeaderSN;

    QString strWavDirectory;
    QString strLastWavStream  = "";
    QString strRcvFrameTag    = "";
    QString strDecodeCapture  = "";

    //--------------------------------------
    // message queue objects
    //--------------------------------------
    QQueue<QString> queTNCStatus;

    QByteArray     *bytDataToSend;       // This is the only copy
    QByteArray     *bytRecvdDataQueue;

    // --------------------------------------
    // GET PARENT OBJECT (modem)
    // --------------------------------------
    void setParentObject(ARDOPModem *obj)    { objMain = obj; connectAndSetupUi(); }

    // --------------------------------------
    //  CONTROL BLOCK FROM PARENT
    // --------------------------------------
    void setMCB(ModemControlBlock *MCB)      { mcb = MCB; }
    void setRCB(RadioControlBlock *RCB)      { rcb = RCB; }

protected:

    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

public slots:
    void setRadioMenuEnabled(bool b);

private slots:
    void slotClose();
    void slotSetupTNC(bool);
    void slotReadMore();  //!< Connected to the AudioInputIO object to get the internally buffered data.
    void slotUpdateVU();  //!< Connected to the update signal of the AudioInputIO Object - update the VU Meter using the AudioInputIO.level() function.
    void slotAudioErr(QString,QString);  //!< Connected to the AudioInput Object to display errors.
    void slotDispTypeChgd();
    void slotVolChgd(int);   //!< Connected to the volume control on the TNC Panel. A change in setting will update the AudioInput object.
    void slotAudioInputOK(); //!< Connected to the AudioInputOK() signal of the AudioInputIO Object - signals the TNC to change the status display.

private:
    // property local vars   //
    bool m_soundIsPlaying;
    //                       //

    QByteArray       m_buffer;

    QAudioFormat     stcSCFormat;
    QAudioFormat     m_format;
    QAudioDeviceInfo devCaptureDevice;
    // Objects on the heap   //
    class MainPrivate;
    MainPrivate  *mwd;

    // controls to display on the dialog that are not native as in QMainWindow
    QMenuBar     *menubar;
    QMenu        *fileMenu;
    QMenu        *setupMenu;
    QMenu        *helpMenu;
    QAction      *actionFileExit;
    QAction      *actionSetupRadio;
    QAction      *actionSetupTNC;
    QAction      *actionHelpIndex;
    // Qt Library Audio Objects
    QAudioInput  *audioInput;
    QAudioOutput *audioOutput;
    QIODevice    *m_input;
    // Sub-classed local object
    AudioInputIO *audioIOObject;  //!< \sa AudioInputIO
    // Modem parent object
    ARDOPModem   *objMain;
    // Setup form for TNC
    TNCSetup     *objTNCSetup;
    // Local Control Block objects used by reference elsewhere
    ModemControlBlock *mcb;
    RadioControlBlock *rcb;


    void initializeLocalVars();
    void initializeFromIni();
    void initializeAudio();
    void createLocalObjects();
    /*! \details
     * The format and capture device must have been set
     * before calling this function since both are needed
     * to create the input object   */
    void createAudioInput();

    void connectAndSetupUi();
    void readSettings();
    void writeSettings();
    void audioInputError(QAudio::Error);
    double getVolumeDb();
};

#endif // TNCPANEL_HPP

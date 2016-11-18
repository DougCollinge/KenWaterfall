#ifndef TCPTEST_HPP
#define TCPTEST_HPP
#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkSession>

#include "ui_tcptest.h"

class QTextTableFormat;

namespace Ui {
class TcpTest;
}

class TcpTest : public QMainWindow
{
    Q_OBJECT

public:
    TcpTest();
    ~TcpTest();

    bool hasConnection(const QHostAddress &senderIp, qint32 senderPort = -1) const;

public slots:
    void appendMessage(const QString &from, const QString &message);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private slots:
    void slotDisplayError(QAbstractSocket::SocketError);
    void slotDisplayProcError(QProcess::ProcessError);
    void slotRequestNewData();
    void slotNewConnection();
    void slotNewMessage();
    void slotRemoveConnection(QString);
    void slotReturnPressed();
    void slotSendPing();
    void slotConnectSvr();
    void ConnectToServer();

private:
    enum NetworkState
    {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };
    enum DataType
    {
        PlainText,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    bool         blnTCPConnected;
    bool         isGreetingMessageSent;

    quint32      tcpport;
    quint32      udpport;
    qint32       numBytesForCurrentDataType;
    qint32       transferTimerId;

    QTextTableFormat tableFormat;

    QString      id;
    QString      myNickName;
    QString      m_tcpaddress;
    QString      greetingMessage;

    QByteArray   buffer;

    QTime        pongTime;

    DataType     currentDataType;
    NetworkState netstate;

    QTimer       *pingTimer;

    QNetworkSession *networkSession;
    QProcess     *myProcess;
    QTcpSocket   *tcpsocket;
    QUdpSocket   *udpSocket;

    Ui::TcpTest  *ui;

    void interpretTCPReply();
    void ReturnPressed(QString);
    void processData();
    void StartServer();
    void readSettings();
};

#endif // TCPTEST_HPP

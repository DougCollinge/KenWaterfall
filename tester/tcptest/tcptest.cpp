#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtCore/QIODevice>
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtGui/QTextCursor>
#include <QtGui/QTextTable>
#include <QtGui/QColor>
#include <QtGui/QTextTableFormat>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkConfiguration>
#include <QtNetwork/QNetworkConfigurationManager>

#include "tcptest.hpp"

static const qint32  BroadcastInterval = 2000;  // 2 sec
static const qint32  PongTimeout       = 60000; // 60 sec
static const qint32  PingInterval      = 5000;  //  5 sec
static const QString SeparatorToken    = " ";

TcpTest::TcpTest() : QMainWindow()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    qDebug() << "Constructor initializing - network session open";
    greetingMessage = tr("undefined");
    myNickName = "TCPTEST";
    netstate = WaitingForGreeting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    tcpport = 8515;
    udpport = tcpport + 1;  // tcp port + 1
    pingTimer = new QTimer(this);
    pingTimer->setInterval(PingInterval);
    connect(pingTimer, SIGNAL(timeout()), this, SLOT(slotSendPing()));

    readSettings();  // get stored settings for network use

    StartServer();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

TcpTest::~TcpTest()
{
    delete ui;
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void TcpTest::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TcpTest::closeEvent(QCloseEvent *event)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    Q_UNUSED(event)
    pingTimer->stop();
    delete pingTimer;

    QByteArray data = "c: ABORT";
    tcpsocket->write(data,data.length());

    myProcess->terminate();
    myProcess->waitForFinished(5000);
    delete myProcess;

    tcpsocket->close();
    delete tcpsocket;
}

void TcpTest::slotConnectSvr()
{
    qDebug() << "Server process started...connecting";
    if ( myNickName == "" ) myNickName = "HOST";
    tcpsocket = new QTcpSocket(this);
    tcpsocket->connectToHost(QHostAddress(m_tcpaddress),tcpport);
    connect(tcpsocket, SIGNAL(disconnected()),tcpsocket, SLOT(deleteLater()));
    connect(tcpsocket, SIGNAL(connected()), this, SLOT(slotNewConnection()));
    connect(tcpsocket, SIGNAL(disconnected()), pingTimer, SLOT(stop()));
    connect(tcpsocket, SIGNAL(readyRead()), this, SLOT(slotNewMessage()));
    bool Ok = tcpsocket->open(QTcpSocket::ReadWrite);
    if ( !Ok )
    {
        qDebug() << "Socket Open Fail -- " << tcpsocket->errorString();
    }
    else
    {
        qDebug() << "Socket open";
    }

    ui = new Ui::TcpTest;
    ui->setupUi(this);

    ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
    ui->textEdit->setFocusPolicy(Qt::NoFocus);
    ui->textEdit->setReadOnly(true);
    ui->btnSend->setEnabled(false);
    connect(ui->btnConn,SIGNAL(clicked(bool)),this,SLOT(slotRequestNewData()));
    connect(ui->btnSend,SIGNAL(clicked(bool)),this,SLOT(slotRequestNewData()));
    qDebug() << "GUI connected - creating TCPSocket";

    myNickName = "HOST";
    qDebug() << myNickName << " connection established as TCP tcpsocket... ";


    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
    connect(ui->btnConn,SIGNAL(clicked(bool)),this,SLOT(slotConnectSvr()));

    qDebug() << "TCP Socket ready to connect";
//    slotNewConnection();

    ui->lblTCP->setEnabled(false);
    ui->btnSend->setEnabled(true);
}

void TcpTest::slotSendPing()
{
    if ( pongTime.elapsed() > PongTimeout )
    {
        abort();
        return;
    }
    appendMessage("HOST","PING 1 p"); // 5 sec ping message
    tcpsocket->write("PING 1 p\0");
}


void TcpTest::appendMessage(const QString &from, const QString &message)
{
    qDebug() << "Message received:" << message;
    if ( from.isEmpty() || message.isEmpty() ) return;

    QTextCursor cursor(ui->textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(1, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText('<' + from + "> ");
    table->cellAt(0, 1).firstCursorPosition().insertText(message);
    QScrollBar *bar = ui->textEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void TcpTest::slotNewConnection()
{
    if ( myNickName.isEmpty() )  return;

    QByteArray data = "c: ";
//    tcpsocket->write(data,data.length());
//    appendMessage(myNickName.toLatin1(), data);
    QColor color = ui->textEdit->textColor();
    ui->textEdit->setTextColor(Qt::gray);
    ui->textEdit->append(tr("* %1 has joined").arg(myNickName));
    ui->textEdit->setTextColor(color);
    ui->listWidget->addItem(myNickName);
}

void TcpTest::slotReturnPressed()
{
    QString text = "";
    text = ui->lineEdit->text().trimmed();
    ReturnPressed(text);
    ui->lineEdit->clear();
}

void TcpTest::slotRemoveConnection(QString nick)
{
    if ( nick.isEmpty() ) return;

    QList<QListWidgetItem *> items = ui->listWidget->findItems(nick,Qt::MatchExactly);

    if ( items.isEmpty() ) return;

    delete items.at(0);
    QColor color = ui->textEdit->textColor();
    ui->textEdit->setTextColor(Qt::gray);
    ui->textEdit->append(tr("* %1 has left").arg(nick));
    ui->textEdit->setTextColor(color);
}

void TcpTest::slotDisplayError(QAbstractSocket::SocketError e)
{
    switch ( e )
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("TCP tcpsocket Test as host"),
                                 tr("The modem Server was not found. Please check the "
                                    "host name and port settings."));
        ui->btnSend->setEnabled(false);
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("TCP tcpsocket test as host"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the modem executable is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        ui->btnSend->setEnabled(false);
        break;
    default:
        ui->btnSend->setEnabled(false);
    }

    ui->btnSend->setEnabled(true);
}

void TcpTest::slotDisplayProcError(QProcess::ProcessError e)
{
    switch ( e )
    {
    case QProcess::UnknownError:
        qDebug() << "Unknown Error Starting process";
        break;
    case QProcess::ReadError:
        qDebug() << tr("Read Error starting process");
        break;
    case QProcess::WriteError:
        qDebug() << tr("Read Error starting process");
        break;
    default:
        qDebug() << tr("Error starting process- ") << myProcess->errorString();
        break;
    }

    ui->btnSend->setEnabled(true);
}

void TcpTest::slotRequestNewData()
{
    ui->btnSend->setEnabled(false);
    ReturnPressed(ui->lineEdit->text().trimmed());
}

void TcpTest::slotNewMessage()
{
    QString msg = tcpsocket->readAll();
    appendMessage("ARDOPMODEM",msg);
}

void TcpTest::ReturnPressed(QString message)
{
    if ( message.isEmpty() ) return;

    QByteArray msg = message.toUtf8();
    QChar c = msg.at(0);
    if ( c == '/' )
    {
        QColor color = ui->textEdit->textColor();
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(tr("! Unknown command: %1").arg(message.left(message.indexOf(' '))));
        ui->textEdit->setTextColor(color);
    }
    else
    {
        QByteArray data = "c: " + msg.trimmed();
        tcpsocket->write(data,data.length());
        appendMessage(myNickName, data);
    }

    ui->lineEdit->setText("");
}

void TcpTest::processData()
{
    buffer = tcpsocket->read(numBytesForCurrentDataType);
    if ( buffer.size() != numBytesForCurrentDataType )
    {
        abort();
        return;
    }

    switch ( currentDataType )
    {
    case PlainText:
//  process new message with username, QString::fromUtf8(buffer);
        break;
    case Ping:
        tcpsocket->write("PONG 1 p");
        break;
    case Pong:
        pongTime.restart();
        break;
    default:
        break;
    }

    currentDataType = Undefined;
    numBytesForCurrentDataType = 0;
    buffer.clear();
}

void TcpTest::ConnectToServer()
{
}

void TcpTest::StartServer()
{
    qDebug() << "Starting ARDOP Server...";
#ifdef Q_OS_LINUX
    QString program = "./ardopd";
#endif
#ifdef Q_OS_WIN
    QString program = "ardopd.exe";
#endif
    QStringList arguments;
    arguments << "";

    myProcess = new QProcess(this);
    myProcess->start(program, arguments);
    slotConnectSvr();
//    connect(myProcess,SIGNAL(started()),this,SLOT(slotConnectSvr()));
    connect(myProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotDisplayProcError(QProcess::ProcessError)));
}

void TcpTest::readSettings()
{
    QDir dir;
    QSettings settings(dir.canonicalPath() + "ardop_tnc.ini",QSettings::IniFormat);
    settings.beginGroup("NETWORK");
      // general settings to local vars here
      id           = settings.value("DefaultNetworkConfiguration").toString();
      m_tcpaddress = settings.value("Address","127.0.0.1").toString();
      tcpport      = settings.value("TCPPort",8515).toInt();
    settings.endGroup();
}

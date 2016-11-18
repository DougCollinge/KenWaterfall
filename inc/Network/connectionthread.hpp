#ifndef CONNECTIONTHREAD_HPP
#define CONNECTIONTHREAD_HPP
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

/*! \ingroup NETWORK
 * \class ConnThread
 * \brief Connection Thread
 * \details
 * This is the QThread class created for each connection
 * to a server. Thread will stay alive so that
 * signal/slot to function properly not dropped out
 * in the middle when thread dies.
 *
 * \note
 * Qt::DirectConnection is used because it's multi-threaded<br />
 * This makes the slot to be invoked immediately, when the
 * signal is emitted.  */
class ConnThread : public QThread
{
    Q_OBJECT

public:
    explicit ConnThread(qintptr ID, QObject *parent = 0);

    void run(); //!< re-implementation of default run() function

signals:
    void newMessage(QString data);
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
};

#endif // CONNECTIONTHREAD_HPP

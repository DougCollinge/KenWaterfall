#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>

/*! \ingroup NETWORK
 * \class TCPServer
 * \brief The server object definition
 * \details
 * This is a model for threaded client/server communication
 * using the Qt QtNetwork class library.
 *
 * A threaded server is created that waits for a client
 * connection.
 * \note
 * Note that Qt requires that the QTcpServer
 * be in the same thread it is used in.
 * So it cannot be declared as a separate class
 * variable.  It must be in the run thread.  */
class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TCPServer(QObject *parent = 0);

    void startServer();

signals:
    void newConnection(qintptr socket);

public slots:

protected:
    /*! \brief incomingConnection slot
     * \details
     * This slot is connected to the
     * newConnection() signal from QTcpServer.
     * Every new connection will be run in
     * a newly created thread. Once a thread
     * is not needed, it will be be deleted later.
     *
     * Connection threads create the TCP socket in the
     * run() function. */
    void incomingConnection(qintptr socketDescriptor);
};


#endif // TCPSERVER_HPP

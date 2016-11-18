#ifndef BUFFEROBJECT_HPP
#define BUFFEROBJECT_HPP
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QByteArray>

/*! \ingroup SOUND
 * \class BufferObject
 * \brief central circular buffer for tx and rx audio
 * \details
 * This is not a template but an object that represents
 * either input or output data (one for each). It is a
 * circular buffer whose size may be set by the setBufSize
 * property function. The size defaults to 1024 but the
 * current size may be retrived with getBufSize property
 * function.
 *
 * There are several static elements of this object:
 *
 * static QWaitCondition bufferNotEmpty;
 * static QWaitCondition bufferNotFull;
 * static QMutex         Mutex;              // mutex locking for buffer
 * static int            NumUsedBytes = 0;
 * static QByteArray    *Buffer;
 */
class BufferObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 m_bufferSize READ getBufSize WRITE setBufSize)
    Q_PROPERTY(quint32 m_buflen     READ length)

public:
    BufferObject(QObject *parent=0);
    ~BufferObject();

    volatile int          NumUsedBytes;

    QWaitCondition        bufferNotEmpty;
    QWaitCondition        bufferNotFull;
    QMutex                mutex;              //!< mutex locking for rx buffer

    QByteArray           *buffer;

    // PROPERTY SETS AND GETS     //
    void setBufSize(quint32 bz)                 { m_bufferSize = bz; }

    quint32 getBufSize()                        { return m_bufferSize; }
    quint32 length()                            { m_buflen = buffer->size(); return m_buflen;     }
    // end property sets and gets //

    void initialize();                        //!< reset buffer to zero and get ready to start use
    char * data()                               { return buffer->data(); }
    const char * constData()                    { return buffer->constData(); }
    void writeBuffer(QByteArray, quint32);


private:
    quint32 m_bufferSize;
    quint32 m_buflen;

    static const int DataSize = 100000;
};

#endif // BUFFEROBJECT_HPP

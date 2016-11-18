#ifndef BUFFEROBJECT_CPP
#define BUFFEROBJECT_CPP

#include "bufferobject.hpp"

static quint32 stwByte;
static quint32 strByte;

BufferObject::BufferObject(QObject *parent) : QObject(parent)
{
    NumUsedBytes = 0;
    stwByte = 0;
    strByte = 0;
    buffer = new QByteArray;
    buffer->clear();
}

BufferObject::~BufferObject()
{
    delete buffer;
}

void BufferObject::initialize()
{
    // not normally called
    // may be called right after creation and setting of size
    // if called later it may destroy latent data.
    // use the assignemnt from the tncpanel instead.
    NumUsedBytes = 0;
    stwByte = 0;
    strByte = 0;
    delete buffer;
    buffer = new QByteArray;
    buffer->clear();
}

void BufferObject::writeBuffer(QByteArray data, quint32 len)
{
    // insert data in the buffer
    // if stByte + len is larger than buffer
    // then write stByte to end of buffer
    // and start at zero to write the rest.
    // mutex locks during write and unlocks for read.
    // (circular buffer method)
    if ( (stwByte + len) > quint32(buffer->size()) )
    {
        quint32 shortBytes = buffer->size() - stwByte;
        mutex.lock();  // lock the buffer during write
        buffer->insert(stwByte,data.left(shortBytes));
        stwByte += shortBytes;
        stwByte = 0;
        quint32 newlen = data.length() - shortBytes;
        buffer->insert(0,data.right(newlen));
        stwByte = newlen;
        mutex.unlock(); // unlock for read
    }
    else
    {
        mutex.lock();
        buffer->insert(stwByte,data);
        mutex.unlock();
        stwByte += data.length();
    }
}

#endif // BUFFEROBJECT_CPP

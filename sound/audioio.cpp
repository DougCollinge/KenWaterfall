#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>
#include <QtCore/QThread>
//#include <QtWidgets/QApplication>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>

#include "audioio.hpp"

AudioIO::AudioIO(QObject *parent) : QIODevice(parent)
{
#ifdef DEBUG_AUDIO
    qDebug() << "Constructing Audio IODevice";
#endif
    numUsedBytes = 0;
    m_maxAmplitude = 0.0;
    m_level        = 0.0;
    formatset      = false;
    // instantiate the central buffer objects
    // this is the only place these objects are instantiated
    RxBuffer = new BufferObject;
    TxBuffer = new BufferObject;
    RxBuffer->setBufSize(4096);
    TxBuffer->setBufSize(4096);
    m_format = QAudioDeviceInfo::defaultInputDevice().preferredFormat();
}

AudioIO::~AudioIO() { }

void AudioIO::setAudioFormat(QAudioFormat format)
{
#ifdef DEBUG_AUDIO
    qDebug() << "Setting Audio Format from params";
    qDebug() << "Audio Format:";
    if ( format.byteOrder() == QAudioFormat::BigEndian )
      qDebug() << "   Byte: BIG ENDIAN";
    else
        qDebug() << "   Byte: LITTLE ENDIAN";
    qDebug() << "   Ch:" << QString("%1").arg(format.channelCount());
    qDebug() << "Codec:" << QString("%1").arg(format.codec());
    qDebug() << "   Sz:" << QString("%1").arg(format.sampleSize());
    qDebug() << " Rate:" << QString("%1").arg(format.sampleRate());
    if ( format.sampleType() == QAudioFormat::SignedInt )
      qDebug() << "  Typ: SignedInt";
    else if ( format.sampleType() == QAudioFormat::UnSignedInt )
        qDebug() << "  Typ: UnSignedInt";
    else if ( format.sampleType() == QAudioFormat::Float )
        qDebug() << "  Typ: Float";
#endif
    m_format.setByteOrder(format.byteOrder());
    m_format.setChannelCount(format.channelCount());
    m_format.setCodec(format.codec());
    m_format.setSampleRate(format.sampleRate());
    m_format.setSampleSize(format.sampleSize());
    m_format.setSampleType(format.sampleType());

    switch ( m_format.sampleSize() )
    {
    case 8:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        break;
    }
    formatset = true; // Now audioIO is ready to use
#ifdef DEBUG_AUDIO
    qDebug() << "Audio Format Set....exiting";
#endif
}

void AudioIO::start()
{
    if ( !formatset )
    { // check to see the format has been set before using
#ifdef DEBUG_AUDIO
        qCritical() << "Trying to start without format set...";
#endif
        return;
    }
    errorString() = "";
    // open for capture

    if ( !open(QIODevice::WriteOnly) )
    {
#ifdef DEBUG_AUDIO
    qDebug() << "Audio IODevice open error..." << errorString();
#endif
        return;
    }
#ifdef DEBUG_AUDIO
    qDebug() << "Audio IODevice open...exiting";
#endif
}

void AudioIO::stop()  { close(); }

qint64 AudioIO::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return 0;
}

qint64 AudioIO::writeData(const char *data, qint64 len)
{
    // write the capture data to the buffers
    // check to see the format has been set before using
    if ( !formatset )
    {
        qCritical() << "Trying to capture without format set...";
        return len;
    }

    // format set so continue
    quint32 i = 0;
    quint32 j = 0;
    quint32 value = 0;
    quint32 maxValue = 0;
    // m_maxAmplitude was set when format was set
    if ( m_maxAmplitude )
    {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);  // is 8 bit
        const quint32 channelBytes = quint32(m_format.sampleSize() / 8);
        const quint32 sampleBytes  = quint32(m_format.channelCount() * channelBytes);
        Q_ASSERT(len % sampleBytes == 0); // is entire length of samples
        const quint32 numSamples = quint32(len / sampleBytes);

        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for ( i = 0; i < numSamples; ++i )
        {
            for ( j = 0; j < quint32(m_format.channelCount()); ++j )
            {
                switch ( m_format.sampleSize() )
                {
                case 8:  // 8 bit samples
                    if ( m_format.sampleType() == QAudioFormat::UnSignedInt )
                    {
                        value = *reinterpret_cast<const quint8*>(ptr);
                    }
                    else  if ( m_format.sampleType() == QAudioFormat::SignedInt )
                    {
                        value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                    }
#ifdef DEBUG_AUDIO
                    //                    qDebug() << "Writing 8 bit value:" << QString("%1").arg(value);
#endif
                    break;

                case 16:  // 16 bit samples
                    if ( m_format.sampleType() == QAudioFormat::UnSignedInt )
                    {  // sample type is 16 bit unsigned int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qFromLittleEndian<quint16>(ptr);
                        else
                            value = qFromBigEndian<quint16>(ptr);
                    }
                    else if ( m_format.sampleType() == QAudioFormat::SignedInt )
                    {  // sample type is 16 bit signed int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qAbs(qFromLittleEndian<qint16>(ptr));
                        else
                            value = qAbs(qFromBigEndian<qint16>(ptr));
                    }
#ifdef DEBUG_AUDIO
                    //                    qDebug() << "Writing 16 bit value:" << QString("%1").arg(value);
#endif
                    break;

                case 32:  // 32 bit samples
                    if ( m_format.sampleType() == QAudioFormat::UnSignedInt )
                    {  // sample type is 32 bit unsigned int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qFromLittleEndian<quint32>(ptr);
                        else
                            value = qFromBigEndian<quint32>(ptr);
                    }
                    else if ( m_format.sampleType() == QAudioFormat::SignedInt )
                    {  // sample type is 32 bit signed int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qAbs(qFromLittleEndian<qint32>(ptr));
                        else
                            value = qAbs(qFromBigEndian<qint32>(ptr));
                    }
                    else if ( m_format.sampleType() == QAudioFormat::Float )
                    {  // sample type is 32 bit float
                        value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
                    }
#ifdef DEBUG_AUDIO
                    //                    qDebug() << "Writing 32 bit value:" << QString("%1").arg(value);
#endif
                    break;

                default:
                    // defaulting to 16 bit
                    if ( m_format.sampleType() == QAudioFormat::UnSignedInt )
                    {  // sample type is 16 bit unsigned int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qFromLittleEndian<quint16>(ptr);
                        else
                            value = qFromBigEndian<quint16>(ptr);
                    }
                    else if ( m_format.sampleType() == QAudioFormat::SignedInt )
                    {  // sample type is 16 bit signed int
                        if ( m_format.byteOrder() == QAudioFormat::LittleEndian)
                            value = qAbs(qFromLittleEndian<qint16>(ptr));
                        else
                            value = qAbs(qFromBigEndian<qint16>(ptr));
                    }
#ifdef DEBUG_AUDIO
                    //                    qDebug() << "Writing default 16 bit value:" << QString("%1").arg(value);
#endif
                    break;
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }
        maxValue = qMin(maxValue, m_maxAmplitude);
    }
    m_level = double(value) / double(m_maxAmplitude);

    numUsedBytes += len;
    if ( m_level < 0 ) m_level = 0;

    emit update();
    return len;
}



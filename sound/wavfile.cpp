#include <QtCore/qendian.h>
#include <QtCore/QVector>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtMultimedia/QAudioProbe>
#include <QtMultimedia/QAudioRecorder>

#include "wavfile.hpp"

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

class WavFile::WavePrivate
{
public:
    bool outputLocationSet;

    qint64 m_headerLength;

    QAudioFormat    m_fileFormat;
};

WavFile::WavFile(TNCPanel *parent) : QFile(parent)
{
    wp = new WavePrivate;

    wp->outputLocationSet = false;
    wp->m_headerLength = 0;

    audioRecorder = new QAudioRecorder(this);
    probe = new QAudioProbe;
    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));

    probe->setSource(audioRecorder);
    //audio devices
}

bool WavFile::open(const QString &fileName)
{
    close();     // to insure only one device is active
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readHeader();
}

const QAudioFormat &WavFile::fileFormat() const { return wp->m_fileFormat; }

qint64 WavFile::headerLength() const { return wp->m_headerLength; }

bool WavFile::readHeader()
{
    seek(0);
    CombinedHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result)
    {
        if (
             (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0  ||
              memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0) &&
              memcmp(&header.riff.type, "WAVE", 4) == 0           &&
              memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0  &&
             (header.wave.audioFormat == 1                        ||
              header.wave.audioFormat == 0)
           )
        {

            // Read off remaining header information
            DATAHeader dataHeader;

            if ( qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader) )
            {
                // Extended data available
                quint16 extraFormatBytes;
                if ( peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16) )   return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if ( read(throwAwayBytes).size() != throwAwayBytes )   return false;
            }

            if ( read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))   return false;

            // Establish format
            if ( memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0 )
                wp->m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                wp->m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            wp->m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            wp->m_fileFormat.setCodec("audio/pcm");
            wp->m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            wp->m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            wp->m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        }
        else
        {
            result = false;
        }
    }
    wp->m_headerLength = pos();
    return result;
}


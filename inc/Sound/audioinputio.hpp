#ifndef AUDIOIO_HPP
#define AUDIOIO_HPP
/*! \file audioio.hpp */
#include <QtCore/QIODevice>
#include <QtCore/QByteArray>
#include <QtMultimedia/QAudioFormat>

#include "ardop_global.hpp"
#include "bufferobject.hpp"

#define PUSH_MODE_LABEL "Enable push mode"
#define PULL_MODE_LABEL "Enable pull mode"
#define SUSPEND_LABEL   "Suspend recording"
#define RESUME_LABEL    "Resume recording"

const qint32 BufferSize = 4096;

/*! \ingroup SOUND
 * \class AudioIO
 * \brief The Audio IO Object
 * \details
 * Qt uses this sub-class of QIOobject to read data from
 * the Audio input selected. Later the application will
 * use the data read for FFT and level indications and
 * other DSP operations.
 *
 * Because Qt Multimedia allows the application to access
 * the raw audio stream, the data can either be inspected
 * or modified by the application.
 * \copyright 2016 AD5XJ ad5xj@arrl.net
 *
 * $QT_BEGIN_LICENSE:BSD$
 * You may use this file under the terms of the BSD license
 * as follows:
 *
 * "Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that the
 * following conditions are met:
 *   * Redistributions of source code must retain the above
 *     copyright notice, this list of conditions and the
 *     following disclaimer.
 *   * Redistributions in binary form must reproduce the
 *     above copyright notice, this list of conditions and
 *     the following disclaimer in the documentation and/or
 *     other materials provided with the distribution.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE."
 *
 * $QT_END_LICENSE$
 */
class AudioInputIO : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(bool formatset READ isFormatSet)          //!< read only
    Q_PROPERTY(QAudioFormat m_format READ format WRITE setAudioFormat)

public:
    AudioInputIO(QObject *parent=0);
    ~AudioInputIO();

    // PROPERTY SETS AND GETS  //
    void setAudioFormat(QAudioFormat format);

    bool isFormatSet()         { return formatset; }
    QAudioFormat format()      { return m_format;  }
    //                         //

    //--------------------------------------
    // data buffer objects
    //--------------------------------------
    BufferObject   *RxBuffer;  //!< static buffer definition for audioInput

    qint64 readData(char *data, qint64 maxlen);     //!< \details dummy procedure to satisfy virtual purity
    qint64 writeData(const char *data, qint64 len); //!< \details write captured data to buffer \returns len of data

    void start();
    void stop();
    qreal level() const { return m_level; }

signals:
    void update();
    void signalAudioInputOK();

public slots:

private:
    bool         formatset;

    quint32      numUsedBytes;
    quint32      m_maxAmplitude;

    double       m_level; // 0.0 <= m_level <= 1.0 or percent of max 1.0 = 100%


    QMutex       mutex;

    QAudioFormat m_format;

    ModemControlBlock *MCB;  //!< local version of object passed to it
};

#endif // AUDIOIO_HPP

#ifndef WAVFILE_HPP
#define WAVFILE_HPP

#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QMediaRecorder>

class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;
class QAudioLevel;

#include "tncpanel.hpp"

/*! \ingroup SOUND
 * \class WavFile
 * \brief Handler for playing and recording .wav files
 * \details
 * \copyright &copy; 2015 The Qt Company Ltd.
 * Contact: http://www.qt.io/licensing/
 *
 * This file is part of the examples of the Qt Toolkit.
 *
 * $QT_BEGIN_LICENSE:BSD$
 * You may use this file under the terms of the BSD
 * license as follows:<br /><br />
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:<br /><br />
 *
 * &nbsp;&nbsp;&bull; Redistributions of source code must retain
 * the above copyright notice, this list of conditions
 * and the following disclaimer.<br />
 * &nbsp;&nbsp;&bull; Redistributions in binary form
 * must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.<br />
 * &nbsp;&nbsp;&bull; Neither the name of The
 * Qt Company Ltd nor the names of its contributors
 * may be used to endorse or promote products derived
 * from this software without specific prior written
 * permission.<br />
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $QT_END_LICENSE$
 *
 * This code is derived and modified from the Qt Library Examples.
 * Although it is different it still holds the above copyright from
 * Digia.
 *
 * \ingroup SOUND
 * \class WavFile
 * \brief A class representing a .wav file
 * \details
 * This class allows a convenient way to read and write .wav
 * files from sound data. It inherits from QFile.
 *                                                   */
class WavFile : public QFile
{
    Q_OBJECT

public:
    explicit WavFile(TNCPanel *parent = 0);
    virtual ~WavFile();

    using QFile::open;
    bool open(const QString &fileName);

    qint64 headerLength() const;
    const QAudioFormat &fileFormat() const;

public slots:
    void processBuffer(const QAudioBuffer&);

private slots:
    void setOutputLocation();
    void togglePause();
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

private:
    class WavePrivate;
    WavePrivate *wp;

    QList<QAudioLevel*> audioLevels;

    QAudioRecorder *audioRecorder;
    QAudioProbe    *probe;

    TNCPanel*       objMain;

     bool readHeader();
     void clearAudioLevels();
};

#endif // WAVFILE_HPP

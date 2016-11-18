#ifndef UTILS_HPP
#define UTILS_HPP
/*! \file utils.hpp */
/*! \ingroup UTILS */
#include <math.h>
using namespace std;

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QByteArray>
#include <QtMultimedia/QAudioFormat>

#include "ardop_global.hpp"

// ------------------------------------------
//          PUBLIC GLOBAL FUNCTIONS
// ------------------------------------------
bool isPCM(const QAudioFormat &format); //!< Check whether the audio format is PCM
bool isPCMS16LE(const QAudioFormat &format);  //!< Check whether the audio format is signed, little-endian, 16-bit PCM

quint8 getHexToInt(QString);

qint16 realToPcm(qreal real); //!< Scale real value in [-1.0, 1.0] to PCM
qint64 audioDuration(const QAudioFormat &format, qint64 bytes);
qint64 audioLength(const QAudioFormat &format, qint64 microSeconds);

qreal nyquistFrequency(const QAudioFormat &format);
qreal pcmToReal(qint16 pcm);  //!< Scale PCM value to [-1.0, 1.0]

QString formatToString(const QAudioFormat &format);

void ArrayCopy(QVarLengthArray<qint32> *, quint32, QVarLengthArray<qint32> *, quint32, quint32);
void ArrayCopy(QVarLengthArray<qint32> *, QVarLengthArray<qint32> *, quint32);
void ArrayCopy(QByteArray *, QByteArray *, quint32);
void ArrayCopy(QByteArray *, QByteArray *, quint64);
void ArrayCopy(QByteArray *, QByteArray *, quint32, quint32);
void ArrayCopy(QByteArray *, quint32,  QByteArray *, quint32, quint32);
void ArrayCopy(QByteArray *, quint64,  QByteArray *, quint64, quint64);


void arrayDump(QString label,  qint16 *data, quint32 len, bool toAux, bool singleColumn);
void arrayDump(QString label,  qint32 *data, quint32 len, bool toAux);
void arrayDump(QString label, quint16 *data, quint32 len, bool inHex, bool toAux);
void arrayDump(QString label, quint32 *data, quint32 len, bool inHex, bool toAux);
void arrayDump(QString label,  float  *data, quint32 len, bool toAux, bool singleColumn);

void clearTuningStats();
void clearQualityStats();
void initializeConnection();
void GenCRC8(QByteArray);
//void GenCRC16(QByteArray);

/*! \brief Subroutine to compute a 16 bit CRC value and append it to the Data...
 * \details
 * For  CRC-16-CCITT = x^16 + x^12 +x^5 + 1  intPoly = 1021 Init FFFF
 * intSeed is the seed value for the shift register and must be in the range 0-&HFFFF
 */
void GenCRC16(QByteArray *, qint32, qint32, qint32 intSeed = 65535);


void GenCRC16FrameType(QByteArray *, qint32, qint32, quint8 bytFrameType = 0);
void UpdateFSKFrameDecodeStats(bool blnDecodeSuccess);
void InitializeConnection();
void ClearQualityStats();
void ClearTuningStats();
void LogStats();

bool CompareByteArrays(QByteArray, QByteArray);
bool CheckValidCallsignSyntax(QString);                           //!< Function for checking valid call sign syntax
bool CheckCRC16FrameType(QByteArray *, quint8 bytFrameType = 0);
bool trash(QString,bool);

quint8 GenerateSessionID(QString, QString);                      //!< function to generate 8 bit session ID

QByteArray Bit6ToASCII(QByteArray);
QByteArray GetBytes(QString);                    //!< Function to convert string Text (ASCII) to byte array

QString GetString(QByteArray, qint32 intFirst = 0, qint32 intLast = -1);
QString DeCompressCallsign(QByteArray);
QString DeCompressGridSquare(QByteArray);
QString ARQStateToString();
QString ARDOPStateToString();

#endif



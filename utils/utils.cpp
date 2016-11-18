#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileDevice>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtMultimedia/QAudioFormat>

#include "utils.hpp"


void LogStats()
{
/*
    try
    {
        qint32 intTotFSKDecodes = stcTuningStats.intGoodFSKFrameDataDecodes + stcTuningStats.intFailedFSKFrameDataDecodes;
        qint32 intTotPSKDecodes = stcTuningStats.intGoodPSKFrameDataDecodes + stcTuningStats.intFailedPSKFrameDataDecodes;
        // to stop warnings in release mode
        Q_UNUSED(intTotFSKDecodes)
        Q_UNUSED(intTotPSKDecodes)

        qInfo() << "\n";
        quint32 msf = QDateTime::currentDateTime().msecsTo(stcConnection.dttSessionStart);
        qInfo() << "******************** ARQ session stats with " << stcConnection.strRemoteCallsign << " (" << QString("%1").arg(msf) << " minutes) ****************************";
        qInfo() << "     Session Bandwidth=" << QString("%1").arg(stcConnection.intSessionBW) << " Hz,  Remote Leader Rcvd=" << QString().arg(stcConnection.intReceivedLeaderLen) << " ms";
        qInfo() << "     LeaderDetects=" << QString("%1").arg(stcTuningStats.intLeaderDetects) << "   AvgLeader S:N(3KHz noise BW)=" << QString("%1").arg(stcTuningStats.dblLeaderSNAvg - 23.8) << "dB  LeaderSyncs=" << QString("%1").arg(stcTuningStats.intLeaderSyncs);
        qInfo() << "     FrameSyncs=" << QString("%1").arg(stcTuningStats.intFrameSyncs) << "   Good Frame Type Decodes=" << QString().arg(stcTuningStats.intGoodFSKFrameTypes) << "  Failed Frame Type Decodes =" << QString("%1").arg(stcTuningStats.intFailedFSKFrameTypes);
        qInfo() << "     Avg Frame Type decode distance=" << QString("%1").arg(stcTuningStats.dblAvgDecodeDistance) << " over " << QString("%1").arg(stcTuningStats.intDecodeDistanceCount) << " decodes";
        double msd = stcConnection.intBytesReceived / int(QDateTime::currentDateTime().msecsTo(stcConnection.dttSessionStart)/1000);
        qInfo() << "     Total Bytes Sent=" << QString("%1").arg(stcConnection.intBytesSent) << "    Total Bytes Received=" << QString("%1").arg(stcConnection.intBytesReceived) << "     Session Throughput=" << QString("%1").arg(int(stcConnection.intBytesSent)) << QString("%1").arg(msd) << " bytes/min";

        if ( (stcTuningStats.intGoodFSKFrameDataDecodes +
              stcTuningStats.intFailedFSKFrameDataDecodes +
              stcTuningStats.intGoodFSKSummationDecodes) > 0
           )
        {
            qInfo() << "\n";
            qInfo() << "  FSK:\n";
            qInfo() << "     Good FSK Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intGoodFSKFrameDataDecodes) + "  RecoveredFSKCarriers with Summation=" + QString("%1").arg(stcTuningStats.intGoodFSKSummationDecodes) + "  Failed FSK Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intFailedFSKFrameDataDecodes);
            qInfo() << "     Accum FSK Tracking=" + QString("%1").arg(stcTuningStats.intAccumFSKTracking) + "  over " + QString("%1").arg(stcTuningStats.intFSKSymbolCnt) + " symbols   Good Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intGoodFSKFrameDataDecodes) + "   Failed Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intFailedFSKFrameDataDecodes);
        }

        if ( (stcTuningStats.intGoodPSKFrameDataDecodes +
              stcTuningStats.intFailedPSKFrameDataDecodes +
              stcTuningStats.intGoodPSKSummationDecodes) > 0
           )
        {
            qInfo() << "\n";
            qInfo() << "  PSK:\n";
            qInfo() << "     Good PSK Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intGoodPSKFrameDataDecodes) + "  Recovered PSK Carriers with Summation=" + QString("%1").arg(stcTuningStats.intGoodPSKSummationDecodes) + "  Failed PSK Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intFailedPSKFrameDataDecodes);
            qInfo() << "     Accum PSK Tracking=" + QString("%1").arg(stcTuningStats.intAccumPSKTracking) + "/" + QString("%1").arg(stcTuningStats.intPSKTrackAttempts) + " attempts  over " + QString("%1").arg(stcTuningStats.intPSKSymbolCnt) + " total PSK Symbols";
        }

        if ( (stcTuningStats.intGoodQAMFrameDataDecodes +
              stcTuningStats.intFailedQAMFrameDataDecodes +
              stcTuningStats.intGoodQAMSummationDecodes) > 0
           )
        {
            qInfo() << "\n";
            qInfo() << "  QAM:\n";
            qInfo() << "     Good QAM Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intGoodQAMFrameDataDecodes) + "  Recovered QAM Carriers with Summation=" + QString("%1").arg(stcTuningStats.intGoodQAMSummationDecodes) + "  Failed QAM Data Frame Decodes=" + QString("%1").arg(stcTuningStats.intFailedQAMFrameDataDecodes);
            qInfo() << "     Accum QAM Tracking=" + QString("%1").arg(stcTuningStats.intAccumQAMTracking) + "/" + QString("%1").arg(stcTuningStats.intQAMTrackAttempts) + " attempts  over " + QString("%1").arg(stcTuningStats.intQAMSymbolCnt) + " total QAM Symbols";
        }

        QString msg = "  Squelch=";
        msg += QString("%1").arg(MCB->RxSquelchLevel);
        msg += "  BusyDet=";
        msg += QString("%1").arg(MCB->BusyDet);
        msg += "  Mode Shift UPs=";
        msg += QString("%1").arg(stcTuningStats.intShiftUPs);
        msg += "   Mode Shift DOWNs=";
        msg += QString("%1").arg(stcTuningStats.intShiftDNs);
        msg += "  Link Turnovers=";
        msg += QString("%1").arg(stcTuningStats.intLinkTurnovers);
        qInfo() << "\n";
        qInfo() << msg;

        qInfo() << "\n";
        qInfo() << "  Received Frame Quality:\n";
        if ( stcQualityStats.int4FSKQualityCnts > 0 )
            qInfo() << "     Avg 4FSK  Quality=" + QString("%1").arg(int(stcQualityStats.int4FSKQuality / stcQualityStats.int4FSKQualityCnts)) + "  on " + QString("%1").arg(stcQualityStats.int4FSKQualityCnts) + " frames";

        if ( stcQualityStats.int8FSKQualityCnts > 0 )
            qInfo() << "     Avg 8FSK  Quality=" + QString("%1").arg(int(stcQualityStats.int8FSKQuality / stcQualityStats.int8FSKQualityCnts)) + "  on " + QString("%1").arg(stcQualityStats.int8FSKQualityCnts) + " frames";

        if ( stcQualityStats.int16FSKQualityCnts > 0 )
            qInfo() << "     Avg 16FSK Quality=" << QString("%1").arg(int(stcQualityStats.int16FSKQuality / stcQualityStats.int16FSKQualityCnts)) + "  on " + QString("%1").arg(stcQualityStats.int16FSKQualityCnts) + " frames";

        if ( stcQualityStats.intPSKQualityCnts.at(0) > 0 )
            qInfo() << "     Avg 4PSK  Quality=" << QString("%1").arg(int(stcQualityStats.intPSKQuality.at(0) / stcQualityStats.intPSKQualityCnts.at(0))) << "  on " << QString("%1").arg(stcQualityStats.intPSKQualityCnts.at(0)) << " frames";

        if ( stcQualityStats.intPSKQualityCnts.at(1) > 0 )
            qInfo()  << "     Avg 8PSK  Quality=" << QString("%1").arg(int(stcQualityStats.intPSKQuality.at(1) / stcQualityStats.intPSKQualityCnts.at(1))) << "  on " << QString("%1").arg(stcQualityStats.intPSKQualityCnts.at(1)) << " frames";

        if ( stcQualityStats.intQAMQualityCnts > 0 )
            qInfo()  << "     Avg 16QAM Quality=" << QString("%1").arg(int(stcQualityStats.intQAMQuality / stcQualityStats.intQAMQualityCnts)) + "  on " << QString("%1").arg(stcQualityStats.intQAMQualityCnts) << " frames";

        qInfo("*******************************************************************************************");
    }
    catch (exception ex)
    {
        qCritical() << "UTILS::LogStats - " << ex.what();
    }
*/
} // LogStats()

quint8 getHexToInt(QString hex)
{
    bool ok = false;
    int iVal = 0;

    iVal = hex.toInt(&ok,16);
    return iVal;
}

// Sub to Initialize before a new Connection
void InitializeConnection()
{
/*
    stcConnection.strRemoteCallsign = "";           //!< remote station call sign
    stcConnection.intBytesSent = 0;                 //!< Outbound bytes confirmed by ACK and squenced
    stcConnection.intBytesReceived = 0;
    stcConnection.intReceivedLeaderLen = 0;         //!< Zero out received leader length (the length of the leader as received by the local station
    stcConnection.intReportedLeaderLen = 0;         //!< Zero out the Reported leader length the length reported to the remote station
    stcConnection.bytSessionID = 255;               //!< Session ID
    stcConnection.blnLastPSNPassed = false;         //!< the last PSN passed True for Odd, False for even.
    stcConnection.blnInitiatedConnection = false;   //!< flag to indicate if this station initiated the connection
    stcConnection.dblAvgPECreepPerCarrier = 0.0;    //!< computed phase error creep
    stcConnection.dttLastIDSent = QDateTime::currentDateTime(); //!< date/time of last ID
    stcConnection.intTotalSymbols = 0;              //!< To compute the sample rate error
    stcConnection.strLocalCallsign = "";            //!< this stations call sign
    stcConnection.intSessionBW = 0;                 // ExtractARQBandwidth()
    stcConnection.intCalcLeader = MCB->LeaderLength; //!<
    stcConnection.dttSessionStart = QDateTime::currentDateTime();

    ClearQualityStats();
    ClearTuningStats();
*/
}  // InitializeConnection()

// Sub to Clear the Quality Stats
void ClearQualityStats()
{
/*
    stcQualityStats.intQAMQuality = 0;
    stcQualityStats.intQAMQualityCnts = 0;
    stcQualityStats.intQAMSymbolsDecoded = 0;
    stcQualityStats.int4FSKQuality = 0;
    stcQualityStats.int4FSKQualityCnts = 0;
    stcQualityStats.int8FSKQuality = 0;
    stcQualityStats.int8FSKQualityCnts = 0;
    stcQualityStats.int16FSKQuality = 0;
    stcQualityStats.int16FSKQualityCnts = 0;
    stcQualityStats.intPSKQuality.resize(2);        //!< Quality for 4PSK, 8PSK  modulation modes
    stcQualityStats.intPSKQualityCnts.resize(2);    //!< Counts for 4PSK, 8PSK modulation modes
    // need to get total quantity of PSK modes
    stcQualityStats.intFSKSymbolsDecoded = 0;
    stcQualityStats.intPSKSymbolsDecoded = 0;
    */
} // ClearQualityStats()


void ClearTuningStats()
{
/*
    stcTuningStats.intLeaderDetects = 0;
    stcTuningStats.intLeaderSyncs = 0;
    stcTuningStats.intFrameSyncs = 0;
    stcTuningStats.intAccumFSKTracking = 0;
    stcTuningStats.intFSKSymbolCnt = 0;
    stcTuningStats.intAccumPSKTracking = 0;
    stcTuningStats.intAccumQAMTracking = 0;
    stcTuningStats.intPSKSymbolCnt = 0;
    stcTuningStats.intQAMSymbolCnt = 0;
    stcTuningStats.intGoodFSKFrameTypes = 0;
    stcTuningStats.intFailedFSKFrameTypes = 0;
    stcTuningStats.intGoodQAMFrameDataDecodes = 0;
    stcTuningStats.intFailedQAMFrameDataDecodes = 0;
    stcTuningStats.intGoodFSKFrameDataDecodes = 0;
    stcTuningStats.intFailedFSKFrameDataDecodes = 0;
    stcTuningStats.intGoodPSKFrameDataDecodes = 0;
    stcTuningStats.intGoodPSKSummationDecodes = 0;
    stcTuningStats.intGoodQAMSummationDecodes = 0;
    stcTuningStats.intGoodFSKSummationDecodes = 0;
    stcTuningStats.intFailedPSKFrameDataDecodes = 0;
    stcTuningStats.intAvgFSKQuality = 0;
    stcTuningStats.intAvgPSKQuality = 0;
    stcTuningStats.dblFSKTuningSNAvg = 0.0;
    stcTuningStats.dblLeaderSNAvg = 0.0;
    stcTuningStats.dblAvgPSKRefErr = 0.0;
    stcTuningStats.intPSKTrackAttempts = 0;
    stcTuningStats.dblAvgDecodeDistance = 0.0;
    stcTuningStats.intDecodeDistanceCount = 0;
    stcTuningStats.intShiftDNs = 0;
    stcTuningStats.intShiftUPs = 0;
    stcTuningStats.intLinkTurnovers = 0;
    stcTuningStats.intEnvelopeCors = 0;
    stcTuningStats.dblAvgCorMaxToMaxProduct = 0.0;
*/
} // ClearTuningStats()

QString ARDOPStateToString()
{
/*
    switch ( ARDOPState )
    {
    case DISC:
        return "DISCONNECTED ";
    case OFFLINE:
        return "OFFLINE ";
    case FECRcv:
        return "FEC Rcv ";
    case FECSend:
        return "FEC Send ";
    case IDLE:
        return "IDLE ";
    case IRStoISS:
        return "TRANSITION FROM IRS TO ISS ";
    case IRS:
        return "IRS ";
    case ISS:
        return "ISS";
    default:
        return "UNKNOWN";
    }
*/
    return "";
}

QString ARQStateToString()
{
/*
    switch ( ARQState )
    {
    case ISSConReq:
        return "ISSConReq ";
    case ISSConAck:
        return "ISSConAck ";
    case ISSData:
        return "ISSData ";
    case ISSId:
        return "ISSId ";
    case IRSConAck:
        return "IRSConAck";
    case IRSData:
        return "IRSData";
    case IRSfromISS:
        return "TRANSITION FORM IRS TO ISS";
    case None:
        return "UNKNOWN";
    default:
        return "UNKNOWN";
    }
*/
    return "";
}


void ArrayCopy(QVarLengthArray<qint32> *srcarray, QVarLengthArray<qint32> *destarray, quint32 xfrsize)
{
    // Copies a range of elements from an Array starting
    // at the stnode element and pastes them into another
    // Array starting at the stnode element. The length
    // is specified as a 32-bit unsigned integer.
    quint32 x = 0;
    // start copying the srcarray from node zero into
    // destarray at node zero, and copy xfrsize
    // ending at (stnode + xfrsize)
    // TODO: test for boundary violations
    //       (e.g. stnode + zfrsize > destarray.size)
    //       (e.g. stnode < 0)
    //       (xfrsize > destarray.size)...etc.
    destarray->clear();
    for ( x = 0; x < (xfrsize - 1); ++x )
    {
        destarray->append(srcarray->at(x));
    }
}

void ArrayCopy(QVarLengthArray<qint32> *srcarray, quint32 len, QVarLengthArray<qint32> *destarray, quint32 stnode, quint32 xfrsize)
{
    // Copies a range of elements from an Array starting
    // at the stnode element and pastes them into another
    // Array starting at the stnode element. The length
    // is specified as a 32-bit unsigned integer.
    quint32 x = 0;
    Q_UNUSED(len)
    // start copying the srcarray from stnode into
    // destarray at stnode, and copy xfrsize
    // ending at (stnode + xfrsize)
    // TODO: test for boundary violations
    //       (e.g. stnode + zfrsize > destarray.size)
    //       (e.g. stnode < 0)
    //       (xfrsize > destarray.size)...etc.
    for ( x = stnode; x < ((stnode + xfrsize) - 1); ++x )
    {
        destarray->insert(x,srcarray->at(x));
    }
}

void ArrayCopy(QByteArray *srcarray, QByteArray *destarray, quint32 blksize)
{
    // Copies a range of elements from an Array starting
    // at the first element and pastes them into another
    // Array starting at the first element. The length
    // is specified as a 32-bit integer.
    quint32 x = 0;
    // start copying the srcarray from startel into
    // destarray at sartel, and copy blksiz elements
    // ending at (startel +blksiz)
    for ( x = 0; x < (blksize - 1); ++x )
    {
        destarray->insert(x, srcarray->at(x));
    }
}

void ArrayCopy(QByteArray *srcarray, QByteArray *destarray, quint64 blksiz)
{
    quint64 x = 0;
    // start copying the srcarray from startel into
    // destarray at sartel, and copy blksiz elements
    // ending at (startel +blksiz)
    for ( x = 0; x < (blksiz - 1); ++x )
    {
        destarray->insert(x,srcarray->at(x));
    }
}

void ArrayCopy(QByteArray *srcarray, quint32 startel, QByteArray *destarray, quint32 stdestel, quint32 siz)
{
    Q_UNUSED(startel)

    quint32 x = 0;
    quint32 y = 0;
    // start copying the srcarray from startel into
    // destarray at stdestel, and copy siz elements
    // ending at (stdestel +siz)
    y = stdestel;
    for ( x = stdestel; x < ((stdestel + siz) - 1); ++x )
    {
        destarray->insert(x,srcarray->at(x));
        ++y;
    }
}

void ArrayCopy(QByteArray *srcarray, QByteArray *destarray, quint32 startel, quint32 blksiz)
{
    quint32 x = 0;
    // start copying the srcarray from startel into
    // destarray at sartel, and copy blksiz elements
    // ending at (startel +blksiz)
    for ( x = startel; x < (startel + blksiz); ++x )
    {
        destarray->insert(x,srcarray->at(x));
    }
}

void ArrayCopy(QByteArray *srcarray, quint64 startel, QByteArray *destarray, quint64 stdestel, quint64 siz)
{
    Q_UNUSED(startel)
    quint64 x = 0;
    quint64 y = 0;
    // start copying the srcarray from startel into
    // destarray at stdestel, and copy siz elements
    // ending at (stdestel +siz)
    y = stdestel;
    for ( x = stdestel; x < ((stdestel + siz) - 1); ++x )
    {
        destarray->insert(x,srcarray->at(x));
        ++y;
    }
}

bool CompareByteArrays(QByteArray *ary1, QByteArray *ary2)
{
    // Function to compare two Byte Arrays
//    if ( !(a && b) ) return true;
//    if ( !(a || b) )  return false;
    qint32 i = 0;

    if ( ary1->length() != ary2->length() ) return false;
    for ( i = 0; i < (ary1->length() - 1); ++i )
    {
        if ( ary1->at(i) != ary2->at(i) ) return false;
    }
    return true;
} // Compare Arrays

void GenCRC8(QByteArray Data)
{
    // Subroutine to compute a 8 bit CRC value and
    // append it to the Data...
    // For  CRC-8-CCITT = x^8 + x^7 +x^3 + x^2 + 1
    // intPoly = 1021 Init FFFF
    bool ok;
    QString hexcsix = "C6";
    QString hexff   = "FF";
    QString hex80   = "80";
    qint32 intReg      = hex80.toUInt(&ok, 16);
    qint32 intPoly     = hexcsix.toUInt(&ok,16); // This implements the CRC polynomial  x^8 + x^7 +x^3 + x^2 + 1
    qint32 intRegister = hexff.toUInt(&ok,16);

    for ( quint16 c = 0; c < sizeof(Data); ++c )
    {
        for ( qint16 i = 7; i > 0; --i )
        {// for each bit processing MS bit first
            quint8 c = Data.at(i);
            quint8 d = pow(2,i);
            bool blnBit = ((c && d) != 0);
            if ( (intRegister && intReg)  == intReg )
            {   // the MSB of the register is set
                // Shift left, place data bit as LSB, then divide
                // Register := shiftRegister left shift 1
                // Register := shiftRegister xor polynomial
                if ( blnBit)
                    intRegister = intRegister && (1 + 2 * intRegister);
                else
                    intRegister = intRegister && (2 * intRegister);
                // xOR intRegister and intPoly
                intRegister = intRegister ^ intPoly;  // Xor the numbers
            }
            else
            {  // the MSB is not set
                // Register is not divisible by polynomial yet.
                // Just shift left and bring current data bit onto LSB of shiftRegister
                if ( blnBit )
                    intRegister = intRegister && (1 + 2 * intRegister);
                else
                    intRegister = intRegister && (2 * intRegister);
            }
        }
    }
    Data.resize(Data.size() +1); //make the Data array one byte larger
    quint16 ff = hexff.toUInt(&ok,16);
    quint16 x  = Data.size() - 1;
    Data[x] = quint8(intRegister && ff); // LS 8 bits of Register
} // GenCRC8

void GenCRC16FrameType(QByteArray *Data, qint32 intStartIndex, qint32 intStopIndex, quint8 bytFrameType)
{  // Subroutine to compute a 16 bit CRC value and
   // append it to the Data... With LS byte XORed by
   // bytFrameType
   // For  CRC-16-CCITT = x^16 + x^12 +x^5 + 1
   // intPoly = 1021 Init FFFF
   // intSeed is the seed value for the shift register
   // and must be in the range 0-&HFFFF
    bool ok;
    QString hex8810 = "8810";
    QString hex8000 = "8000";
    QString hexffff = "FFFF";
    qint32 reg8000     = hex8000.toUInt(&ok,16);
    qint32 intPoly     = hex8810.toUInt(&ok,16);     // This implements the CRC polynomial  x^16 + x^12 +x^5 + 1
    qint32 intRegister = hexffff.toUInt(&ok,16); // initialize the register to all 1's

    for ( qint16 j = intStartIndex; j < intStopIndex; ++j )
    {
        for ( qint16 i = 7; i > 0; --i )
        {  // for each bit processing MS bit first
            quint8 c = Data->at(j);
            quint8 d = pow(2,i);
            bool blnBit = ((c && d) != 0);
            if ( (intRegister && reg8000) == reg8000 )
            {  // the MSB of the register is set
               // Shift left, place data bit as LSB, then divide
               // Register := shiftRegister left shift 1
               // Register := shiftRegister xor polynomial
                if ( blnBit )
                    intRegister = hexffff.toUInt(&ok,16) && (1 + 2 * intRegister);
                else
                    intRegister = hexffff.toUInt(&ok,16) && (2 * intRegister);
                // XOr intRegister and intPoly
                intRegister = intRegister ^ intPoly;
            }
            else
            {   // the MSB is not set
                // Register is not divisible by polynomial yet.
                // Just shift left and bring current data bit onto LSB of shiftRegister
                if ( blnBit )
                    intRegister = hexffff.toUInt(&ok,16) && (1 + 2 * intRegister);
                else
                    intRegister = hexffff.toUInt(&ok,16) && (2 * intRegister);
            }
        }
    }
    // Put the two CRC bytes after the stop index
    QString hexff00 = "FF00";
    QString hexff   = "FF";
    const char b1 = quint8((intRegister && hexff00.toUInt(&ok,16)) / 256);
    const char b2 = quint8((intRegister && hexff.toUInt(&ok,16)) ^ bytFrameType);
    quint16 ndx1 = intStopIndex + 1;
    Data[ndx1]     = &b1; // MS 8 bits of Register
    Data[ndx1 + 1] = &b2; // LS 8 bits of Register
} // GenCRC16

void GenCRC16(QByteArray *Data, qint32 intStartIndex, qint32 intStopIndex, qint32 intSeed)
{
    qint32 intPoly     = 34832; // This implements the CRC polynomial  x^16 + x^12 +x^5 + 1
    qint32 intRegister = intSeed;
    qint8  i = 0;
    qint32 j = 0;

    for ( j = intStartIndex; j < intStopIndex; ++j )
    {
        for ( i = 7; i > 0; --i )
        {
            // for each bit processing MS bit first
            bool blnBit = ((Data->at(j) && pow(i,2.0)) != 0 );
            if ( intRegister && 32768 )
            {
                // the MSB of the register is set
                // Shift left, place data bit as LSB, then divide
                // Register := shiftRegister left shift 1
                // Register := shiftRegister xor polynomial
                if ( blnBit )
                    intRegister = 65535 && (1 + 2 * intRegister);
                else
                    intRegister = 65535 && (2 * intRegister);

                intRegister = intRegister ^ intPoly;
            }
            else
            {
                // the MSB is not set
                // Register is not divisible by polynomial yet.
                // Just shift left and bring current data bit onto LSB of shiftRegister
                if ( blnBit )
                    intRegister = 65535 && (1 + 2 * intRegister);
                else
                    intRegister = 65535 && (2 * intRegister);
            }
        } // Next i
    } // Next j
    // Put the two CRC bytes after the stop index
    qint64 x = intRegister & int(65280 / 256);
    Data[intStopIndex + 1] = QString("%1").arg((x,10)).toLatin1(); // MS 8 bits of Register
    Data[intStopIndex + 2] = QString("%1").arg((intRegister & 255),10).toLatin1(); // LS 8 bits of Register
} // GenCRC16

bool CheckValidCallsignSyntax(QString strCallsign)
{
    QString strTestCS    = strCallsign.trimmed().toUpper();
    qint16 intDashIndex  = strTestCS.lastIndexOf("-");
    QString strValidChar = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if ( intDashIndex == -1 )
    {
        // no -SSID
        if ( (strTestCS.length() > 7) ||
             (strTestCS.length() < 3)
           ) return false;

        for ( quint32 i = 0; i < quint32(strCallsign.trimmed().length() - 1); ++i )
        {
            if ( strValidChar.lastIndexOf(strTestCS.mid(i, 1)) == -1 ) return false;
        }
        return true;
    }
    else
    {
        QString strCallNoSSID = strTestCS.mid(0, intDashIndex).trimmed();
        if ( (strCallNoSSID.length() > 7) ||
             (strCallNoSSID.length() < 3)
           ) return false;

        QString strSSID = strTestCS.mid(intDashIndex + 1).trimmed();
        if ( strSSID.toInt() >= 0 )
        {
            if ( (strSSID.toInt() < 0) ||
                 (strSSID.toInt() > 15)
               )
                return false;
            else
                return true;
        }
        if ( strSSID.length() != 1 ) return false;
        if ( QString("ABCDEFHIJKLMNOPQRSTUVWXYZ").lastIndexOf(strSSID) == -1 ) return false;
    }
    return true;
} // CheckValidCallsignSyntax()

bool CheckCRC16FrameType(QByteArray *Data, quint8 bytFrameType)
{
    // Function to compute a 16 bit CRC value and check it against the last 2 bytes of Data (the CRC) XORing LS byte with bytFrameType..
    // Returns True if CRC matches, else False
    // For  CRC-16-CCITT = x^16 + x^12 +x^5 + 1
    // intPoly = 1021 Init FFFF
    // intSeed is the seed value for the shift
    // register and must be in the range 0-&HFFFF

    bool ok;
    QString hexFF   = "FF";
    QString hex8810 = "8810";
    QString hex8000 = "8000";
    QString hexFF00 = "FF00";
    QString hexffff = "FFFF";
    quint16 j = 0;
    quint16 i = 0;
    qint16 intFF       = hexFF.toUInt(&ok,16);
    qint16 int8000     = hex8000.toUInt(&ok,16);
    qint16 intFF00     = hexFF00.toUInt(&ok,16);
    qint16 intFFFF     = hexffff.toUInt(&ok,16);
    qint32 intPoly     = hex8810.toUInt(&ok,16);      // This implements the CRC polynomial  x^16 + x^12 +x^5 + 1
    qint32 intRegister = hexffff.toUInt(&ok,16); // initialize the register to all 1's

    for ( j = 0; j < (sizeof(Data) - 3); ++j )
    {  // 2 bytes short of data length
        for ( i = 7; i > 0; --i )
        {  // for each bit processing MS bit first
            quint8 c = Data->at(j);
            quint8 d = pow(2,i);
            bool blnBit = ((c && d) != 0);
            if ( (intRegister && int8000) == int8000 )
            {   // the MSB of the register is set
                // Shift left, place data bit as LSB, then divide
                // Register := shiftRegister left shift 1
                // Register := shiftRegister xor polynomial
                if ( blnBit )
                    intRegister = intFFFF && (1 + 2 * intRegister);
                else
                    intRegister = intFFFF && (2 * intRegister);
                // Xor intRegister and intPoly
                intRegister = intRegister ^ intPoly;
            }
            else
            {   // the MSB is not set
                // Register is not divisible by polynomial yet.
                // Just shift left and bring current data bit onto LSB of shiftRegister
                if ( blnBit )
                    intRegister = intFFFF && (1 + 2 * intRegister);
                else
                    intRegister = intFFFF && (2 * intRegister);
            }
        }
    }

    // Compare the register with the last two bytes of Data (the CRC)
    const char b1 = quint8((intRegister && intFF00) / 256);
    const char b2 = quint8((intRegister && intFF) ^ bytFrameType);
    if ( Data->at(Data->size() - 2) == b1 )
    {
        if ( Data->at(Data->size() - 1) == b2 ) return true;
    }
    return false;
} // CheckCRC16FrameType

QByteArray Bit6ToASCII(QByteArray byt6Bit)
{
    // Input must be 6 bytes which represent
    // packed 6 bit characters that well
    // result will be 8 ASCII character set
    // values from 32 to 95...
    quint16 i = 0;
    quint16 j = 0;
    qint64 intSum = 0;
    qint64 intMask = 0;
    QByteArray bytReturn;

    for ( i = 0; i <= 2; ++i )
    {
        intSum = 256 * intSum + byt6Bit.at(i);
    }

    for ( j = 0; j <= 3; ++j )
    {
        intMask = int(63 * ((pow(64,double(3 - j)))));
        const char x = int((32 + (intSum && intMask)) / int((pow(64,double(3 - j)))));
        bytReturn[j] = x;
    }

    for ( i = 0; i <= 2; ++i )
    {
        intSum = 256 * intSum + byt6Bit.at(i + 3);
    }

    for ( j = 0; j <= 3; ++j )
    {
        intMask = int(63 * (pow(64, double(3 - j))));
        const char x = int((32 + (intSum && intMask)) / int((pow(64, double(3 - j)))));
        bytReturn[j + 4] = x;
    }
    return bytReturn;
}

QByteArray GetBytes(QString strText)
{
    // Converts a text string to a byte array...
    // this function is built-in to the Qt QString and
    // QByteArray objects. See the docs

    QByteArray bytBuffer;
    bytBuffer.clear();
    bytBuffer.resize(strText.length() - 1);
    quint32 y = bytBuffer.length() - 1;

    for ( quint32 x = 0; x < y; ++x )
    {
        bytBuffer.append(strText.mid(x, 1).toLatin1());
    }
    return bytBuffer;
} // GetBytes()


quint8 GenerateSessionID(QString strCallingCallSign, QString strTargetCallsign)
{
    Q_UNUSED(strCallingCallSign)
    Q_UNUSED(strTargetCallsign)
/*
    quint8 mGenerateSessionID = 0;

    QByteArray bytToCRC = ::GetBytes(strCallingCallSign.toUpper().trimmed() + strTargetCallsign.toUpper().trimmed());
    GenCRC8(bytToCRC);
    quint32 l = bytToCRC.length() - 1;
    if ( bytToCRC.at(l) != 255 )
        mGenerateSessionID = bytToCRC.at(l);
    else // rare case where the computed session ID woudl be FF
        mGenerateSessionID = 0; //Remap a SessionID of FF to 0...FF reserved for FEC mode

    if ( MCB->DebugLog )
    {
        QString msg = "[GenerateSessionID] Caller=";
        msg += strCallingCallSign;
        msg += ",  Target=";
        msg += strTargetCallsign;
        msg += " ID= H";
        msg += QString("%1").arg(mGenerateSessionID);
        qDebug() << msg;
    }
    return mGenerateSessionID;
*/
    return 0;
}


QString GetString(QByteArray bytBuffer, qint32 intFirst, qint32 intLast)
{
    // Function to Get ASCII string from a byte array
    // Converts a byte array to a text string...

    if ( intFirst > bytBuffer.size() )   return "";
    if ( (intLast = -1) || (intLast > bytBuffer.size()) )
    {
         intLast = bytBuffer.size();
    }
    QString sbdInput = "";
    quint16 intIndex = 0;
    for ( intIndex = intFirst; intIndex <= intLast; ++intIndex )
    {
        if ( bytBuffer.at(intIndex) != 0 )
            sbdInput.append(bytBuffer.at(intIndex));
    }
    return sbdInput;
}


QString DeCompressCallsign(QByteArray bytCallsign)
{
    // Function to decompress 6 byte call sign to 7 characters plus optional -SSID of -0 to -15 or -A to -Z
    if ( bytCallsign.size() == 0 ) return "";
    if ( bytCallsign.size() != 6 ) return "";

    bool ok;
    QByteArray bytTest = Bit6ToASCII(bytCallsign);
    QString strWithSSID = "";
    QString callsign = "";
    QString c = QString(bytTest.at(7));

    if ( c.toUInt(&ok,10) ==  48 )
    {
        // Value of "0" so No SSID
        bytTest.resize(6);
        callsign = GetString(bytTest);
        return callsign;
    }
    else if ( (c.toUInt(&ok,10) >= 58) &&
              (c.toUInt(&ok,10) <= 63)
            )
    {
        // handles special case for -10 to -15
        callsign = GetString(bytTest);
        callsign = callsign.mid(0, 7).trimmed() + "-";
        quint8 ix = c.toInt(&ok,10);
        callsign += QString(ix - 48);
        return callsign;
    }
    else
    {
        strWithSSID = GetString(bytTest);
        QString msg = strWithSSID.mid(0, 7);
        msg += "-";
        msg += strWithSSID.mid(0,(strWithSSID.length() - 1));
        return msg;
    }
    return "";
} // DeCompressCallsign()

QString DeCompressGridSquare(QByteArray bytGS)
{
    // Function to decompress 6 byte
    // Grid square to 4, 6 or 8 characters
    if ( bytGS.size() == 0 ) return "";
    if ( bytGS.size() != 6 ) return "";
    QByteArray bytTest = Bit6ToASCII(bytGS);
    return GetString(bytTest);
} // DeCompressGridSquare()

qreal pcmToReal(qint16 pcm)   { return qreal(pcm) / PCMS16MaxAmplitude; }

qint16 realToPcm(qreal real)  { return real * PCMS16MaxValue; }

bool isPCM(const QAudioFormat &format) { return (format.codec() == "audio/pcm"); }

bool isPCMS16LE(const QAudioFormat &format)
{
    return isPCM(format) &&
           format.sampleType() == QAudioFormat::SignedInt &&
           format.sampleSize() == 16 &&
           format.byteOrder() == QAudioFormat::LittleEndian;
}

QString formatToString(const QAudioFormat &format)
{
    QString result;

    if ( QAudioFormat() != format )
    {
        if ( format.codec() == "audio/pcm" )
        {
            QString formatType;

            Q_ASSERT(format.sampleType() == QAudioFormat::SignedInt);

            const QString formatEndian = (format.byteOrder() == QAudioFormat::LittleEndian) ? QString("LE") : QString("BE");

            switch ( format.sampleType() )
            {
            case QAudioFormat::SignedInt:
                formatType = "signed";
                break;
            case QAudioFormat::UnSignedInt:
                formatType = "unsigned";
                break;
            case QAudioFormat::Float:
                formatType = "float";
                break;
            case QAudioFormat::Unknown:
                formatType = "unknown";
                break;
            }

            QString formatChannels = QString("%1 channels").arg(format.channelCount());
            switch ( format.channelCount() )
            {
            case 1:
                formatChannels = "mono";
                break;
            case 2:
                formatChannels = "stereo";
                break;
            }

            result = QString("%1 Hz %2 bit %3 %4 %5")
                .arg(format.sampleRate())
                .arg(format.sampleSize())
                .arg(formatType)
                .arg(formatEndian)
                .arg(formatChannels);
        }
        else
        {
            result = format.codec();
        }
    }

    return result;
}

bool trash(QString filename,bool forceDelete)
{
    QString tmp;
    QFile orgFile(filename);
    QFileInfo modifiedFileInfo(filename);
    QFileInfo info(filename);
    QFile infoFile;
    QFile modifiedFile;
    QDir trDir;
    QDir infoDir;
    QDir filesDir;
    QString infoTxt;
    trDir.setPath(getenv("XDG_DATA_HOME"));
    if ( trDir.path().isEmpty() )
    {
        QString trPath = QDir::toNativeSeparators(QDir::homePath() + "/.local/share/Trash");
        trDir.setPath(trPath);
    }
    QString infoPath = QDir::toNativeSeparators(trDir.path() + "/info");
    infoDir.setPath(infoPath);
    QString filesPath = QDir::toNativeSeparators(trDir.path() + "/files");
    filesDir.setPath(filesPath);
    QString infofilePath = QDir::toNativeSeparators(infoDir.path() + "/" + info.fileName() + ".trashinfo");
    infoFile.setFileName(infofilePath);
    QString modfilePath = QDir::toNativeSeparators(filesDir.path() + "/" + modifiedFileInfo.fileName());
    modifiedFile.setFileName(modfilePath);
    int counter=0;
    do
    {
        if( !modifiedFile.exists() ) break;

        ++counter;
        tmp = QString("%1/%2_%3.%4").arg(filesDir.path()).arg(modifiedFileInfo.completeBaseName()).arg(QString::number(counter)).arg(modifiedFileInfo.suffix());
        modifiedFile.setFileName(tmp);
        tmp = QString("%1/%2_%3.%4").arg(infoDir.path()).arg(modifiedFileInfo.completeBaseName()).arg(QString::number(counter)).arg(modifiedFileInfo.suffix());
        infoFile.setFileName(tmp + ".trashinfo");
    }
    while(1);

    infoTxt = QString("[Trash Info]\nPath=%1\nDeletionDate=%2")
            .arg(filename).arg(QDateTime::currentDateTime().toString(Qt::ISODate));

    if ( (!trDir.exists()) ||
         (!infoDir.exists()) ||
         (!filesDir.exists())
       )
    {
        qCritical() << QApplication::tr("[UTILS]::trash -- Trash folder or one of its components does not exist");
        if(forceDelete) orgFile.remove();
        return false;
    }

    if(!infoFile.open(QIODevice::WriteOnly))
    {
        qCritical() << QApplication::tr("[UTILS]::trash -- Trash folder: can't open") << infoFile.fileName() << QApplication::tr("for writing");
        if(forceDelete) orgFile.remove();
        return false;
    }
    infoFile.write(infoTxt.toLatin1().data());
    infoFile.close();
    QFile trashFile(info.absoluteFilePath());
    QString target;
    target=QString("%1").arg(modifiedFile.fileName());
    if(!trashFile.rename(filename,target))
    {
        qCritical() << QApplication::tr("[UTILS]::trash -- Trash folder: can't rename ") << filename << " " << target;
        if ( forceDelete ) orgFile.remove();
        return false;
    }
    return true;
}  // trash()


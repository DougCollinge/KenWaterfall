#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
/*! \struct TuningStats
 * \details
 * Structure used for statistics used to evaluate
 * and measure performance */
struct TuningStats
{
    qint32 intLeaderDetects;
    qint32 intLeaderSyncs;
    qint32 intAccumLeaderTracking;
    qint32 intAccumQAMTracking;
    qint32 intGoodFSKFrameTypes;
    qint32 intFailedFSKFrameTypes;
    qint32 intAccumFSKTracking;
    qint32 intFSKSymbolCnt;
    qint32 intQAMSymbolCnt;
    qint32 intGoodPSKFrameDataDecodes;
    qint32 intGoodFSKFrameDataDecodes;
    qint32 intGoodQAMFrameDataDecodes;
    qint32 intFailedFSKFrameDataDecodes;
    qint32 intFailedQAMFrameDataDecodes;
    qint32 intAvgFSKQuality;
    qint32 intFrameSyncs;
    qint32 intGoodPSKSummationDecodes;
    qint32 intGoodFSKSummationDecodes;
    qint32 intGoodQAMSummationDecodes;
    qint32 intAccumPSKLeaderTracking;
    qint32 intPSKTrackAttempts;
    qint32 intQAMTrackAttempts;
    qint32 intAccumPSKTracking;
    qint32 intPSKSymbolCnt;
    qint32 intFailedPSKFrameDataDecodes;
    qint32 intAvgPSKQuality;
    qint32 intDecodeDistanceCount;
    qint32 intShiftUPs;
    qint32 intShiftDNs;
    qint32 intLinkTurnovers;
    qint32 intEnvelopeCors;
    double dblAvgCorMaxToMaxProduct;
    double dblFSKTuningSNAvg;
    double dblLeaderSNAvg;
    double dblAvgPSKRefErr;
    double dblAvgDecodeDistance;
};  // TuningStats

/*! \struct QualityStats
 * \details Structure used for Quality stats */
struct QualityStats
{
    qint32 intQAMQuality;
    qint32 intQAMQualityCnts;
    qint32 intQAMSymbolsDecoded;
    qint32 int4FSKQuality;
    qint32 int4FSKQualityCnts;
    qint32 int8FSKQuality;
    qint32 int8FSKQualityCnts;
    qint32 int16FSKQuality;
    qint32 int16FSKQualityCnts;
    qint32 intFSKSymbolsDecoded;
    qint32 intPSKSymbolsDecoded;
    QVarLengthArray<qint32,1> intPSKQuality;
    QVarLengthArray<qint32,1> intPSKQualityCnts;
};  // QualityStats

/*! \struct SessionStats
 * \details  Structure for Session Statistics */
struct SessionStats
{
    bool      blnStatsValid;
    qint32    intTotalBytesSent;
    qint32    intTotalBytesReceived;
    qint32    intFrameTypeDecodes;
    qint32    intMax1MinThruput;
    qint32    intGearShifts;
    double    dblSearchForLeaderAvg;
    double    modeQuality[];
    QDateTime dttSessionStart;
}; //  SessionStats


#endif // PROTOCOL_HPP

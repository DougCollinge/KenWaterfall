#include <math.h>

#include "channel.hpp"

Channel::Channel(QObject *parent)
{

}

void Channel::LPF3000_12000SR(QVarLengthArray<double> *dblSamples, QVarLengthArray<double> *dblFiltered, bool blnInit)
{
    // default for blnInit = true;
    // Assumes sample rate is 12000 and
    // dblCoeff are the half coef mirrored about the last coeff
    static QVarLengthArray<double> dblRegister; // holds the history
    static qint32 intPtr = 0;

    qint32 intRegPtr = 0;
    double dblSum = 0.0;

    if ( blnInit )
    {
        dblRegister.resize(dblLPF_3000.length() * 2 - 2);
        intPtr = 0;
    }
    dblFiltered->resize(dblSamples->length() - 1);
    for ( qint32 i = 0; i < (dblSamples->length() - 1); ++i )
    {
        dblSum = 0.0;
        dblRegister[intPtr] = dblSamples->at(i);
        for ( qint32 j = 0; j < (dblRegister.length() - 1); ++j )
        {
            intRegPtr = intPtr - j;
            if ( intRegPtr < 0 ) intRegPtr += dblRegister.length(); // circular pointer
            if ( j < dblLPF_3000.length() )
                dblSum += (dblLPF_3000.at(j) * dblRegister.at(intRegPtr));
            else
                dblSum += (dblLPF_3000.at((2 * dblLPF_3000.length() - 1) - j) * dblRegister.at(intRegPtr));
        } // Next j
        dblFiltered[i] = dblSum;
        ++intPtr;
        if ( intPtr >= dblRegister.length() ) intPtr = 0; // circular buffer
    } // Next i
}

double Channel::erf(double dblZ)
{
    // Status: Tested and appears to give close values
    // May be able to reduce number of iterations at
    // some degredation in accuracy
    // e.g. with 21 terms erf(2) = .997  with 10 terms erf(2) = .97
    double mErf     = dblZ;
    double dblZSq   = pow(dblZ,2);
    double dblZPwr  = dblZ;
    double dblNfact = 1.0;

    if ( dblZ > 2.0 )
    {
        mErf = dblErf2 + ((dblZ - 2) / dblZ) * (1 - dblErf2);
        // an approximation for the tail where
        // the series doesn't converger well
        return mErf;
    }
    else if (dblZ < -2.0 )
    {
        // an approximation for the tail where
        // the series doesn't converger well
        mErf = -(dblErf2 + double((dblZ + 2) / dblZ) * (1 - dblErf2));
        return mErf;
    }

    // Use Taylor series for 2<= dblZ <= 2
    for ( quint8 i = 1; i < 20; ++i )
    {
        // 21 total terms
        dblNfact = dblNfact * double(i);
        dblZPwr  = dblZPwr * dblZSq;
        if ( (i % 2) = 0 )
            mErf += dblZPwr / (dblNfact * (2 * i + 1));
        else
            mErf -= dblZPwr / (dblNfact * (2 * i + 1));
    } // Next i
    mErf =  mErf * double(2 / sqrt(M_PI));
    return mErf;
} // Erf()

double Channel::log2(double dblX)
{
    double Log2 = log(dblX) / dblLn2;
    return Log2;
} // Log2()

double Channel::normal(double dblX)
{
    double Normal = 0.5 + 0.5 * erf(dblX / dblSQRT2);
    return Normal;
} // Normal()

double Channel::GenRMSNoise(double dblRMSValue)
{
    // The value 9.642 determined emperically
    // to normalize a RMS output of filtered (Pink) noise.
    double val = abs(dblRMSValue);
    double rnd = rand();
    double noise = (9.642 * val * normal(rnd - 0.5) - 0.5);
    return noise;
} // GenRMSNoise()

QVarLengthArray<double> AddPinkNoiseToFloat(QVarLengthArray<double> dblSignal, qint32 SNdB, qint32 intIgnoreLeader)
{
    // Determine the RMS value of dblSignal
    // Add the pink noise 0 to 3000 Hz to reach SNdB (power)
    // Return the dblSamples with noise with 1024 samples of noise leader
    // intIgnoreLeader allows ignoring the first
    // part of the leader in computing the Pink Noise RMS level

    // Compute the RMS value of dblSignal
    double dblRMSSum    = 0.0;
    double dblRMSSignal = 0.0;
    double dblWhiteSum  = 0.0;
    double dblWhiteRMS  = 0.0;
    double dblPinkNoiseRMS = 0.0;
    double dblRatio     = 0.0;
    double dblSigPlusNoiseRMS = 0.0;

    QVarLengthArray<double> dblSigPlusNoise;
    QVarLengthArray<double> dblPink;
    QVarLengthArray<double> dblWhite;

    for ( qint32 i = intIgnoreLeader; i < (dblSignal.length() - 1); ++i )
    {
        dblRMSSum += double((dblSignal(i) ^ 2) / dblSignal.length());
    }
    dblRMSSignal = sqrt(dblRMSSum);
    dblSigPlusNoise.resize(dblSignal.length() + 1023); // Make 1024 samples larger than signal
    dblPink.resize(dblSignal.length() + 1023);
    dblWhite.resize(dblSignal.length() + 1023);

    if ( SNdB > 30 ) return dblSignal;

    for ( qint32 i = 0; i < (dblWhite.length() - 1); ++i )
    {
        dblWhite[i] = GenRMSNoise(dblRMSSignal);
        dblWhiteSum += double(pow(dblWhite.at(i), 2) / dblWhite.length());
    } // Next i
    dblWhiteRMS = sqrt(dblWhiteSum);

    // LPF the white to get pink
    LPF3000_12000SR(dblWhite, dblPink);

    if ( SNdB > 0 )
        dblRatio = pow(10, double(SNdB / 20)); // Calculate the voltage ratio of Signal RMS  to Noise RMS
    else
        dblRatio = 1 / (pow(10, double(-SNdB / 20)));

    for ( qint32 i = 0; i < (dblSigPlusNoise.length() - 1); ++i )
    {
        if ( (i < 1024) || (i >= (dblSignal.length() + 1024)) )
        {
            dblSigPlusNoise[i] = dblPink.at(i);
            // clamp signal to insure no overload of sound card
            // assumes audio data is 16bit signed int
            if ( dblSigPlusNoise.at(i) > 32700 )
                dblSigPlusNoise[i] = 32700;
            else if ( dblSigPlusNoise.at(i) < -32700 )
                dblSigPlusNoise[i] = -32700;
        }
        else
        {
            if ( dblRatio >= 1 )
                dblSigPlusNoise[i] = (dblSignal.at(i - 1024) * dblRatio + dblPink.at(i)) / (dblRatio + 1);
            else
                dblSigPlusNoise(i) = (dblSignal.at(i - 1024) + (dblPink.at(i) / dblRatio)) / (1 + (1 / dblRatio));

            // clamp signal to insure no overload of sound card
            // assumes audio data is 16 bit signed int
            if ( dblSigPlusNoise.at(i) > 32700 )
                dblSigPlusNoise[i] = 32700;
            else if ( dblSigPlusNoise.at(i) < -32700 )
                dblSigPlusNoise[i] = -32700;

            dblSigPlusNoiseRMS += (pow(dblSigPlusNoise.at(i), 2.0) / dblSigPlusNoise.length());
        }

        // clamp signal to insure no overload of sound card
        // assumes audio data is 16 bit signed int
        if ( dblPink.at(i) > 32700 )
            dblPink[i] = 32700;
        else if ( dblPink.at(i) < -32700 )
            dblPink[i] = -32700;

        dblPinkNoiseRMS += (pow(dblPink.at(i), 2) / dblPink.length());
    } // Next i
    dblPinkNoiseRMS = sqrt(dblPinkNoiseRMS);
    dblSigPlusNoiseRMS = sqrt(dblSigPlusNoiseRMS);
    //qDebug("AddPinkNoisetoFloat: Sig/Pink Noise (pwr) = " + QString("%1").arg(20 * log10(dblRMSSignal / sqrt(dblPinkNoiseRMS / dblPink.Length)),6,10) + " dB");
    return dblSigPlusNoise;
}


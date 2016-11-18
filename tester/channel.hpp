#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <math.h>

#include <QtCore/QObject>
#include <QtCore/QVarLengthArray>

/*! \class Channel
 * \brief  Pink Noise Generator for testing
 * \details
 * This class just used for testing to add Pink Noise
 * to the test frames transmitted on the Test Form. */
class Channel : public QObject
{
    Q_OBJECT

public:
    explicit Channel(QObject *parent=0);
    virtual ~Channel();

private:
    double dblSQRT2 = sqrt(2);
    double dblLog2E = 1.4426950408889634;
    double dblLn2   = log(2);
    double dblErf2  = erf(2);

    // These are the half coefficients of
    // a 35 tap FIR Lowpass filter
    // 12000 sample rate
    // Passband upper freq = 3000,
    // stopband Lower freq = 4000,
    // Ripple .5 dB Attn 80 dB (Simple Parks McClean)

    // Use this to filer white noise to get to "pink"
    // in 0 - 3000 Hz bandwidth

    QVarLengthArray<double> dblLPF_3000 =
    {
            0.0025546434097917618,  0.0079080255867991934,
            0.0058328730664431909, -0.0059583900581947112,
           -0.0052880264690653824,  0.010591677144543607,
            0.0042672811976305122, -0.017882797330781967,
            0.000255932825403501,   0.027385809443113113,
           -0.011539003243671417,  -0.037926607961169333,
            0.034960636005051844,   0.047680692955390985,
           -0.086799518656023145,  -0.054614907106485748,
            0.31148884905113489,    0.55713392255360739
    };


    void LPF3000_12000SR(QVarLengthArray<double> *, QVarLengthArray<double> *, bool blnInit = true);
    double erf(double dblZ);                                                                         //!< Taylor series approximation for the Error function (to approcimate the native Erf Function)
    double log2(double dblX);                                                                        //!< Function to return the Log base 2 of a value (not native to VB)
    double normal(double dblX);                                                                      //!< Function to compute the normal distribution
    double GenRMSNoise(double dblRMSValue);                                                          //!< Generates a white noise function (flat frequency) with RMS value ~ dblRMSValue and Avg ~ 0
    QVarLengthArray<double> AddPinkNoiseToFloat(QVarLengthArray<double> *, qint32, qint32);          //!< Function to add Pink Noise (0 to 3 KHz) to signal
};

#endif // CHANNEL_HPP



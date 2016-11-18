#ifndef FFTW3WRAPPER_HPP
#define FFTW3WRAPPER_HPP
/*! \file */
#include <fftw3.h>
#include <math.h>

#include <QtCore/QObject>

/*! \ingroup SOUND
 * \class fftw3calc
 * \brief Wrapper for the FFTW3 Fast Fourier Transform library
 * \details
 * This class is used when FFT calculations are necessary for
 * FFT or specturm display or DSP use. */
class fftw3calc
{

public:
    fftw3calc();
    ~fftw3calc();

    double *out;

    void init(qint32 length, qint32 nblocks, qint32 isamplingrate);
    void realFFT(double *data);

private:
    quint32 blocks;
    quint32 blockIndex;

    qint32 windowSize;
    qint32 fftLength;
    qint32 samplingrate;

    double *hammingBuffer;
    double *dataBuffer;

    fftw_plan plan;

    void createHamming();
    void doFFT();
};


#endif // FFTW3WRAPPER_HPP

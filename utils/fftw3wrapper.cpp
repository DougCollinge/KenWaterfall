#include "fftw3wrapper.hpp"

fftw3calc::fftw3calc()
{
    plan = NULL;
    out  = NULL;
    dataBuffer = NULL;
}

fftw3calc::~fftw3calc()
{
    if ( plan ) fftw_destroy_plan(plan);
    if ( out )  fftw_free(out);
    if ( dataBuffer ) fftw_free(dataBuffer);
}

void fftw3calc::init(qint32 length, qint32 nblocks, qint32 isamplingrate)
{
    windowSize = length;
    fftLength  = windowSize * nblocks;
    blocks     = nblocks;
    blockIndex = 0;

    createHamming();

    samplingrate = isamplingrate;

    //prepare fft
    if ( plan ) fftw_destroy_plan(plan);
    if ( out )  fftw_free(out);
    if ( dataBuffer ) fftw_free(dataBuffer);
    out        = (double *)fftw_malloc(fftLength * sizeof(double));
    dataBuffer = (double *)fftw_malloc(fftLength * sizeof(double));
    // create the fftw plan
    plan = fftw_plan_r2r_1d(fftLength, dataBuffer, out, FFTW_R2HC, FFTW_ESTIMATE);
}

void fftw3calc::createHamming()
{
    qint32 i;

    hammingBuffer = new double[fftLength];
    for ( i = 0; i < fftLength; ++i )
    {
        hammingBuffer[i] = 0.54 - (0.46 * cos(2 * M_PI * ((double)i / ((double)(fftLength - 1)))));
    }

}

void fftw3calc::realFFT(double *data)
{
    qint32 i;
    qint32 j;

    for ( i = 0, j = windowSize * blockIndex; i < windowSize; ++i, ++j )
    {
        dataBuffer[j] = data[i] * hammingBuffer[i];
    }
    doFFT();
}

void fftw3calc::doFFT()
{
    ++blockIndex;
    if ( blockIndex < blocks ) return;
    blockIndex = 0;
    fftw_execute(plan);
}



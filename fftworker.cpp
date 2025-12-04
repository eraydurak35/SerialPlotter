#include "fftworker.h"
#include "kissfft-master\kiss_fft.h"
#include <cmath>

FFTWorker::FFTWorker(int windowSize, double sampleRate)
    : windowSize(windowSize), sampleRate(sampleRate) {

    buffer.reserve(windowSize);
    cfg = kiss_fft_alloc(windowSize, 0, nullptr, nullptr);

    freqs.reserve(windowSize/2);
    mags.reserve(windowSize/2);
}

FFTWorker::~FFTWorker() {
    if (cfg)
        free(cfg);
}

void FFTWorker::stop() {
    buffer.clear();
}

void FFTWorker::addSample([[maybe_unused]]int channel, qint64 timestamp, double value) {

    buffer.append(value);

    if (buffer.size() >= windowSize) {
        computeFFT();
        buffer.clear();
    }
}

void FFTWorker::computeFFT() {

    static QVector<kiss_fft_cpx> in(windowSize);
    static QVector<kiss_fft_cpx> out(windowSize);

    // --- 1) HANNING WINDOW ---
    for (int i = 0; i < windowSize; i++) {
        double w = 0.5 * (1.0 - cos(2.0 * M_PI * i / (windowSize - 1)));
        in[i].r = buffer[i] * w;
        in[i].i = 0;
    }

    // --- 2) FFT ---
    kiss_fft(cfg, in.data(), out.data());

    // --- 3) Amplitude normalization & dB ---
    for (int i = 0; i < windowSize/2; i++) {
        double freq = (double)i * sampleRate / windowSize;

        // raw FFT magnitude
        double raw = sqrt(out[i].r*out[i].r + out[i].i*out[i].i);

        // normalize amplitude
        double mag = (2.0 / windowSize) * raw;

        // convert to dB
        double db = 20.0 * log10(mag + 1e-12);

        freqs.append(freq);
        mags.append(db);
    }

    emit fftReady(freqs, mags);

    freqs.clear();
    mags.clear();
}

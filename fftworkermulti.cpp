#include "fftworkermulti.h"

FFTWorkerMulti::FFTWorkerMulti(QObject *parent)
    : QObject(parent) {
    cfg = kiss_fft_alloc(windowSize, 0, nullptr, nullptr);

    in.resize(windowSize);
    out.resize(windowSize);
}

FFTWorkerMulti::~FFTWorkerMulti() {
    if (cfg)
        free(cfg);
}

void FFTWorkerMulti::enableChannel(int ch) {
    activeChannels.insert(ch);

    if (!channelBuffers.contains(ch))
        channelBuffers[ch].reserve(windowSize);
}

void FFTWorkerMulti::disableChannel(int ch) {
    activeChannels.remove(ch);
}

void FFTWorkerMulti::addSample(int channel, qint64 timestamp, double value) {
    if (!activeChannels.contains(channel))
        return;

    auto &buf = channelBuffers[channel];
    buf.append(value);

    if (buf.size() >= windowSize)
    {
        computeFFT(channel);
        buf.clear();
    }
}

void FFTWorkerMulti::computeFFT(int channel) {
    auto &buf = channelBuffers[channel];
    auto &freqs = channelFreqs[channel];
    auto &mags = channelMags[channel];

    int N = buf.size();
    // Remove DC offset
    double mean = 0.0;
    for (int i = 0; i < N; ++i)
        mean += buf[i];
    mean /= N;

    for (int i = 0; i < N; ++i)
        buf[i] -= mean;

    freqs.clear();
    mags.clear();
    freqs.resize(windowSize/2);
    mags .resize(windowSize/2);

    for (int i = 0; i < windowSize; i++)
    {
        double w = 0.5*(1 - cos(2*M_PI*i/(windowSize-1)));
        in[i].r = buf[i] * w;
        in[i].i = 0;
    }

    kiss_fft(cfg, in.data(), out.data());

    for (int i = 0; i < windowSize/2; i++)
    {
        double freq = (double)i * sampleRate / windowSize;
        double raw = sqrt(out[i].r*out[i].r + out[i].i*out[i].i);
        double mag = (2.0 / windowSize) * raw;
        double db = 20 * log10(mag + 1e-12);

        freqs[i] = freq;
        mags[i]  = db;
    }

    emit fftReadyMulti(channelFreqs, channelMags);
}


void FFTWorkerMulti::stop() {
    // Yeni kanal eklenmesini engelle
    activeChannels.clear();

    // Bufferları temizle
    for (auto &buf : channelBuffers)
        buf.clear();

    // FFT sonuçlarını temizle
    for (auto &f : channelFreqs)
        f.clear();

    for (auto &m : channelMags)
        m.clear();
}

#include "fftworkermulti.h"
#include "qdebug.h"

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

void FFTWorkerMulti::addSample(DataPacket packet) {

    for (int i = 0; i < packet.values.size(); i++) {

        if (!activeChannels.contains(i))
            return;

        auto &buf = channelBuffers[i];
        static qint64 buff_start_timestamp = 0;

        if (i == 0 && buf.size() == 0) {
            buff_start_timestamp = packet.timestamp;
        }

        buf.append(packet.values[i]);

        if (buf.size() >= windowSize) {

            // Sadece 0. sinyal üzerinden sample rate hesapla
            if (i == 0) {
                double time_difference = static_cast<double>(packet.timestamp) - static_cast<double>(buff_start_timestamp);
                if (time_difference > 0) {
                    sampleRate = (windowSize / time_difference) * 1000.0;
                } else {
                    qDebug() << "Why the hell time difference is not bigger than zero";
                }

                buff_start_timestamp = packet.timestamp;
            }
            computeFFT(i);
            buf.clear();
        }
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

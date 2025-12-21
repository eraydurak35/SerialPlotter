#ifndef LATENCYESTIMATOR_H
#define LATENCYESTIMATOR_H

#include <QVector>

class LatencyEstimator
{
public:
    LatencyEstimator();

    // Ayarlar
    void setSampleRate(float fs);
    void setWindowSize(int samples);   // correlation window
    void setMaxLag(int samples);        // max delay to search
    void setDecimation(int decim);      // kaç sample'da bir hesap

    // Yeni örnek ekle
    // return true -> yeni latency hesaplandı
    bool pushSample(float raw, float dsp);

    // Son sonuçlar
    float latencySeconds() const;
    float quality() const;

    // Reset
    void reset();

private:
    // Parametreler
    float sampleRate = 1.0f;
    int windowSize   = 256;
    int maxLag       = 64;
    int decimation   = 20;

    // History
    QVector<float> rawHist;
    QVector<float> dspHist;

    int counter = 0;

    // Son sonuç
    float lastLatencySec = 0.0f;
    float lastQuality    = 0.0f;

    // Core algorithm
    float estimateDelaySamples(const QVector<float>& x,
                             const QVector<float>& y,
                             int maxLag,
                             float &qualityOut);

    float refineLagParabolic(const QVector<float>& corr, int k);
};

#endif // LATENCYESTIMATOR_H

#include "latencyestimator.h"
#include <cmath>

LatencyEstimator::LatencyEstimator()
{
}

void LatencyEstimator::setSampleRate(float fs)
{
    sampleRate = fs;
}

void LatencyEstimator::setWindowSize(int samples)
{
    windowSize = samples;
    reset();
}

void LatencyEstimator::setMaxLag(int samples)
{
    maxLag = samples;
}

void LatencyEstimator::setDecimation(int decim)
{
    decimation = decim;
}

void LatencyEstimator::reset()
{
    rawHist.clear();
    dspHist.clear();
    counter = 0;
    lastLatencySec = 0.0f;
    lastQuality = 0.0f;
}

bool LatencyEstimator::pushSample(float raw, float dsp)
{
    rawHist.push_back(raw);
    dspHist.push_back(dsp);

    if (rawHist.size() > windowSize)
        rawHist.remove(0, rawHist.size() - windowSize);
    if (dspHist.size() > windowSize)
        dspHist.remove(0, dspHist.size() - windowSize);

    counter++;

    if (rawHist.size() < windowSize)
        return false;

    if (counter % decimation != 0)
        return false;

    float q = 0.0f;
    float lagSamples = estimateDelaySamples(rawHist, dspHist, maxLag, q);

    lastQuality = q;

    if (q > 0.02f) {
        lastLatencySec = lagSamples / sampleRate;
        return true;
    }

    return false;
}

float LatencyEstimator::latencySeconds() const
{
    return lastLatencySec;
}

float LatencyEstimator::quality() const
{
    return lastQuality;
}

float LatencyEstimator::estimateDelaySamples(
    const QVector<float>& x,
    const QVector<float>& y,
    int maxLag,
    float &qualityOut)
{
    int N = std::min(x.size(), y.size());
    if (N < 32) {
        qualityOut = 0.0f;
        return 0;
    }

    // ---- Differencing (DC + trend removal) ----
    QVector<float> dx(N - 1), dy(N - 1);
    for (int i = 1; i < N; ++i) {
        dx[i - 1] = x[i] - x[i - 1];
        dy[i - 1] = y[i] - y[i - 1];
    }

    int M = dx.size();
    if (M < 32) {
        qualityOut = 0.0f;
        return 0;
    }

    // Enerji
    double ex = 0.0, ey = 0.0;
    for (int i = 0; i < M; ++i) {
        ex += dx[i] * dx[i];
        ey += dy[i] * dy[i];
    }

    if (ex < 1e-9 || ey < 1e-9) {
        qualityOut = 0.0f;
        return 0;
    }

    int L = std::min(maxLag, M - 1);

    QVector<float> corr(L + 1);

    // ---- Normalized cross-correlation ----
    for (int lag = 0; lag <= L; ++lag) {
        double c = 0.0;
        int count = M - lag;

        for (int i = 0; i < count; ++i)
            c += dx[i] * dy[i + lag];

        corr[lag] = float(c / std::sqrt(ex * ey));
    }

    // ---- Tepe bul ----
    int bestK = 0;
    float best = corr[0];
    float second = -1e9f;

    for (int k = 1; k <= L; ++k) {
        if (corr[k] > best) {
            second = best;
            best = corr[k];
            bestK = k;
        } else if (corr[k] > second) {
            second = corr[k];
        }
    }

    // ---- Quality metriği ----
    qualityOut = best - second;

    // ---- Sub-sample refinement ----
    float refinedLag = refineLagParabolic(corr, bestK);

    return refinedLag;
}


float LatencyEstimator::refineLagParabolic(
    const QVector<float>& corr, int k)
{
    if (k <= 0 || k >= corr.size() - 1)
        return float(k);

    float c1 = corr[k - 1];
    float c2 = corr[k];
    float c3 = corr[k + 1];

    float denom = (c1 - 2.0f * c2 + c3);
    if (std::fabs(denom) < 1e-9f)
        return float(k);

    float delta = 0.5f * (c1 - c3) / denom;
    return float(k) + delta;
}

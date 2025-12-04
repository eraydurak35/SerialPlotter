#pragma once
#include "kissfft-master/kiss_fft.h"
#include <QObject>
#include <QVector>

class FFTWorker : public QObject
{
    Q_OBJECT

public:
    FFTWorker(int windowSize, double sampleRate);
    ~FFTWorker();

public slots:
    void addSample(int channel, qint64 timestamp, double value);
    void stop();

signals:
    void fftReady(const QVector<double> &freqs,
                  const QVector<double> &mags);

private:
    void computeFFT();
    int windowSize;
    double sampleRate;
    QVector<double> freqs;
    QVector<double> mags;

    kiss_fft_cfg cfg = nullptr;

    QVector<double> buffer;
};

#ifndef FFTWORKERMULTI_H
#define FFTWORKERMULTI_H

#include "kissfft-master/kiss_fft.h"
#include <QObject>
#include <QMap>
#include <QSet>
#include "datapacket.h"

class FFTWorkerMulti : public QObject
{
    Q_OBJECT

public:
    explicit FFTWorkerMulti(QObject *parent = nullptr);
    ~FFTWorkerMulti();

public slots:
    void addSample(DataPacket packet);
    void enableChannel(int ch);
    void disableChannel(int ch);
    void stop();

signals:
    void fftReadyMulti(const QMap<int, QVector<double>> &freqs,
                       const QMap<int, QVector<double>> &mags);

private:
    void computeFFT(int channel);

    int windowSize = 1024;
    double sampleRate = 1000.0;

    kiss_fft_cfg cfg = nullptr;

    QSet<int> activeChannels;

    QMap<int, QVector<double>> channelBuffers;
    QMap<int, QVector<double>> channelFreqs;
    QMap<int, QVector<double>> channelMags;

    QVector<kiss_fft_cpx> in, out;
};

#endif // FFTWORKERMULTI_H

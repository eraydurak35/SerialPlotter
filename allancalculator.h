#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class AllanCalculator : public QObject
{
    Q_OBJECT

public:
    struct ChannelResult
    {
        QVector<double> tau;
        QVector<double> allanDev;

        double arw = 0.0;
        double tauArw = 0.0;

        double bias = 0.0;
        double tauBias = 0.0;
    };

    struct Result
    {
        QVector<ChannelResult> channels;
    };

    explicit AllanCalculator(QObject *parent = nullptr);
    double computeARW(const QVector<double>& tau, const QVector<double>& adev, double& tauArw);
    // gyro → °/√s veya rad/√s
    // accel → m/s²/√Hz

    double computeBiasInstability(const QVector<double>& tau, const QVector<double>& adev, double& tauBias);

    void process(const QString& csvPath);

signals:
    void progress(int);
    void finished(const Result&);
    void error(const QString&);

private:
    bool loadCsv(const QString& filePath, int channelIndex);
    ChannelResult compute();

    QVector<double> m_time;
    QVector<double> m_data;
    double m_Ts = 0.0;
};

#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class AllanCalculator : public QObject
{
    Q_OBJECT

public:
    struct Result
    {
        QVector<double> tau;
        QVector<double> allanDev;
    };

    explicit AllanCalculator(QObject *parent = nullptr);
    double computeARW(const QVector<double>& tau, const QVector<double>& adev);
    // gyro → °/√s veya rad/√s
    // accel → m/s²/√Hz

    double computeBiasInstability(const QVector<double>& tau, const QVector<double>& adev, double& tauBias);

public slots:
    void process(const QString& csvPath, int channelIndex);

signals:
    void progress(int percent);
    void finished(const AllanCalculator::Result& result);
    void error(const QString& msg);

private:
    bool loadCsv(const QString& filePath, int channelIndex);
    Result compute();

    QVector<double> m_time;
    QVector<double> m_data;
    double m_Ts = 0.0;
};

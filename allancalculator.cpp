#include "allancalculator.h"
#include "qdebug.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QtMath>

AllanCalculator::AllanCalculator(QObject *parent)
    : QObject(parent)
{
}


bool AllanCalculator::loadCsv(const QString& filePath, int channelIndex)
{
    m_time.clear();
    m_data.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    if (in.atEnd())
        return false;

    in.readLine(); // header

    while (!in.atEnd())
    {
        QStringList t = in.readLine().split(',');
        if (t.size() < channelIndex + 2)
            continue;

        bool okT, okV;
        double ts = t[0].toDouble(&okT) * 1e-3;
        double v  = t[channelIndex + 1].toDouble(&okV);

        if (okT && okV)
        {
            m_time.append(ts);
            m_data.append(v);
        }
    }

    if (m_time.size() < 20)
        return false;

    double sumDt = 0.0;
    for (int i = 1; i < m_time.size(); ++i)
        sumDt += (m_time[i] - m_time[i - 1]);

    m_Ts = sumDt / (m_time.size() - 1);

    qDebug() << "m ts val " << m_Ts;
    return m_Ts > 0.0;
}


AllanCalculator::ChannelResult AllanCalculator::compute()
{
    ChannelResult r;

    const qsizetype N = m_data.size();
    if (N < 20 || m_Ts <= 0.0)
        return r;

    /* ---------- Prefix Sum ---------- */
    QVector<double> prefix(N + 1, 0.0);
    for (qsizetype i = 0; i < N; ++i)
        prefix[i + 1] = prefix[i] + m_data[i];

    /* ---------- Allan computation ---------- */
    double m = 1.0;

    while (true)
    {
        qsizetype mi = static_cast<qsizetype>(qRound(m));
        if (mi < 1)
            mi = 1;

        if (mi > N / 2)
            break;

        const double tau = mi * m_Ts;
        const qsizetype M = N - 2 * mi + 1;
        if (M <= 0)
            break;

        double sum = 0.0;

        for (qsizetype k = 0; k < M; ++k)
        {
            const double a1 =
                (prefix[k + mi] - prefix[k]) / mi;

            const double a2 =
                (prefix[k + 2 * mi] - prefix[k + mi]) / mi;

            const double d = a2 - a1;
            sum += d * d;
        }

        r.tau.append(tau);
        r.allanDev.append(qSqrt(sum / (2.0 * M)));

        m *= std::sqrt(2.0);
    }

    return r;

}

void AllanCalculator::process(const QString& csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit error("Dosya acilamadi");
        return;
    }

    QTextStream in(&file);

    QString header = in.readLine();
    QStringList cols = header.split(',');

    const int channelCount = cols.size() - 1; // timestamp hariç
    QVector<QVector<double>> data(channelCount);

    QVector<double> timestamps;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(',');
        if (parts.size() < cols.size())
            continue;

        timestamps.append(parts[0].toDouble());

        for (int i = 0; i < channelCount; ++i)
            data[i].append(parts[i + 1].toDouble());
    }

    file.close();

    double Ts = (timestamps[1] - timestamps[0]) * 1e-3;

    Result result;
    result.channels.resize(channelCount);

    for (int ch = 0; ch < channelCount; ++ch)
    {
        m_data = data[ch];
        m_Ts = Ts;

        result.channels[ch] = compute();
        double tau_bias_instability = 0;
        double tau_awr = 0;
        result.channels[ch].arw = 60.0 * computeARW(result.channels[ch].tau, result.channels[ch].allanDev, tau_awr);
        result.channels[ch].bias = 3600.0 * computeBiasInstability(result.channels[ch].tau, result.channels[ch].allanDev, tau_bias_instability);
        result.channels[ch].tauBias = tau_bias_instability;
        result.channels[ch].tauArw = tau_awr;

        emit progress(100 * (ch + 1) / channelCount);
    }

    emit finished(result);
}

double AllanCalculator::computeARW(
    const QVector<double>& tau,
    const QVector<double>& adev,
    double& tauArw)
{
    QVector<double> arwCandidates;
    QVector<double> tauCandidates;

    for (int i = 1; i < tau.size(); ++i)
    {
        double slope =
            (qLn(adev[i]) - qLn(adev[i - 1])) /
            (qLn(tau[i]) - qLn(tau[i - 1]));

        if (qAbs(slope + 0.5) < 0.1)
        {
            arwCandidates.append(adev[i] * qSqrt(tau[i]));
            tauCandidates.append(tau[i]);
        }
    }

    if (arwCandidates.isEmpty())
    {
        tauArw = 0.0;
        return 0.0;
    }

    // Ortalama
    double sum = 0.0;
    for (double v : arwCandidates)
        sum += v;

    tauArw = tauCandidates[tauCandidates.size() / 2]; // temsilci τ
    return sum / arwCandidates.size();
}

double AllanCalculator::computeBiasInstability(
    const QVector<double>& tau,
    const QVector<double>& adev,
    double& tauBias)
{
    if (adev.isEmpty())
        return 0.0;

    int idxMin = 0;
    for (int i = 1; i < adev.size(); ++i)
    {
        if (adev[i] < adev[idxMin])
            idxMin = i;
    }

    tauBias = tau[idxMin];
    return adev[idxMin] / 0.664;
}

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


AllanCalculator::Result AllanCalculator::compute()
{
    Result r;
    qsizetype step = 0;

    const qsizetype N = m_data.size();
    if (N < 20 || m_Ts <= 0.0)
        return r;

    /* ---------- Prefix Sum ---------- */
    QVector<double> prefix(N + 1, 0.0);
    for (qsizetype i = 0; i < N; ++i) {

        prefix[i + 1] = prefix[i] + m_data[i];

        emit progress(static_cast<int>(100.0 * (++step) / N));
    }
    /* ---------- Tau setup ---------- */


    for (qsizetype m = 1; m <= N / 2; m *= 2)
    {
        const double tau = m * m_Ts;
        const qsizetype M = N - 2 * m + 1;
        if (M <= 0)
            break;

        double sum = 0.0;

        for (qsizetype k = 0; k < M; ++k)
        {
            const double a1 =
                (prefix[k + m] - prefix[k]) / m;

            const double a2 =
                (prefix[k + 2 * m] - prefix[k + m]) / m;

            const double d = a2 - a1;
            sum += d * d;
        }

        r.tau.append(tau);
        r.allanDev.append(qSqrt(sum / (2.0 * M)));

    }

    emit progress(100);
    return r;
}

void AllanCalculator::process(const QString& csvPath, int channelIndex)
{
    if (!loadCsv(csvPath, channelIndex))
    {
        emit error("CSV okunamadi");
        return;
    }

    Result r = compute();
    emit finished(r);
}


double AllanCalculator::computeARW(
    const QVector<double>& tau,
    const QVector<double>& adev)
{
    QVector<double> arwCandidates;

    for (int i = 1; i < tau.size(); ++i)
    {
        double slope =
            qLn(adev[i]) - qLn(adev[i - 1]);
        slope /= (qLn(tau[i]) - qLn(tau[i - 1]));

        // -0.5 eğimine yakın mı?
        if (qAbs(slope + 0.5) < 0.1)
        {
            double arw = adev[i] * qSqrt(tau[i]);
            arwCandidates.append(arw);
        }
    }

    if (arwCandidates.isEmpty())
        return 0.0;

    // Ortalama
    double sum = 0.0;
    for (double v : arwCandidates)
        sum += v;

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

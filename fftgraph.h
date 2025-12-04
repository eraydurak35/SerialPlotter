#pragma once

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include "fftworker.h"

class FFTGraph : public QWidget
{
    Q_OBJECT

public:
    explicit FFTGraph(QWidget *parent = nullptr);
    ~FFTGraph();

private:
    void updatePlot(const QVector<double> &freqs, const QVector<double> &mags);
    void closeEvent(QCloseEvent *event);

    QChart *chart;
    QChartView *view;
    QLineSeries *series;

    QValueAxis *axisX;
    QValueAxis *axisY;

    int windowSize = 1024;
    double sampleRate = 1000.0; // Hz — ayarlanabilir

    QThread *workerThread = nullptr;
    FFTWorker *worker = nullptr;

signals:
    void newSample(int channel, qint64 timestamp, double value);
    void stopWorker();
    void graphClosed(FFTGraph* self);

public slots:
    void onNewData(int channel, qint64 timestamp, double value);
};

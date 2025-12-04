#include "fftgraph.h"
#include <QVBoxLayout>
#include <QThread>

FFTGraph::FFTGraph(QWidget *parent)
    : QWidget(parent) {

    chart = new QChart();
    series = new QLineSeries();
    chart->addSeries(series);

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    axisX->setTitleText("Frequency (Hz)");
    axisY->setTitleText("Magnitude (db)");
    axisY->setRange(-120, 0);   // tipik dB aralığı

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    view = new QChartView(chart);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);

    resize(800, 400);
    setWindowTitle("FFT Graph");

    workerThread = new QThread(this);
    worker = new FFTWorker(windowSize, sampleRate);

    worker->moveToThread(workerThread);


    // Worker → UI
    connect(worker, &FFTWorker::fftReady,
            this, &FFTGraph::updatePlot);

    // UI → Worker
    connect(this, &FFTGraph::newSample,
            worker, &FFTWorker::addSample);

    connect(this, &FFTGraph::stopWorker,
            worker, &FFTWorker::stop);

    workerThread->start();
}

FFTGraph::~FFTGraph() {
    // 1) Worker’ın çalışmayı bırakması için sinyal
    emit stopWorker();

    // 2) Thread’i durdur
    workerThread->quit();

    // 3) Thread’in bitmesini bekle
    workerThread->wait();

    // 4) Worker ve thread’i sil
    worker->deleteLater();
    workerThread->deleteLater();
}

void FFTGraph::closeEvent(QCloseEvent *event) {

    emit graphClosed(this);
    QWidget::closeEvent(event);
}

void FFTGraph::onNewData(int channel, qint64 timestamp, double value) {

    if (channel == 4) {
        emit newSample(channel, timestamp, value);
    }
}

void FFTGraph::updatePlot(const QVector<double> &freqs,
                          const QVector<double> &mags) {
    series->clear();

    for (int i = 0; i < freqs.size(); i++)
        series->append(freqs[i], mags[i]);

    axisX->setRange(0, freqs.last());
    axisY->setRange(0, *std::max_element(mags.begin(), mags.end()));
}

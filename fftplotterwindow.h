#ifndef FFTPLOTTERWINDOW_H
#define FFTPLOTTERWINDOW_H

#include "qcustomplot/qcustomplot.h"
#include <QWidget>
#include "fftworkermulti.h"

namespace Ui {
class FFTPlotterWindow;
}

class FFTPlotterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FFTPlotterWindow(QWidget *parent = nullptr);
    ~FFTPlotterWindow();

    void addChannel(int channel);
    void setGraphIndex(int idx) {
        setWindowTitle(QString("FFT Graph %1").arg(idx));
    }

private:
    Ui::FFTPlotterWindow *ui;

    void closeEvent(QCloseEvent *event);
    void createSignalSelector(int count);
    void removeChannel(int channel);

    FFTWorkerMulti *fftworker = nullptr;
    QThread *fftworkerthread = nullptr;
    QMap<int, QCPGraph*> graphMap;
    int max_channel_count = 0;
    double xRange = 5.0;  // 5 saniyelik kaydırma
    double yRange = 1.0;
    double yOffset = 0.0;

    QMap<int, QCheckBox*> channelCheckboxes;

    QMap<int, QColor> channelColors;

    QSet<int> selectedChannels;    // FFT penceresinde aktif sinyaller

public slots:
    void onNewData(int channel, qint64 timestamp, double value);

    void onFftReadyMulti(const QMap<int, QVector<double>> &freqs,
                         const QMap<int, QVector<double>> &mags);

signals:
    void addNewFFTData(int channel, qint64 timestamp, double value);
    void graphClosed(FFTPlotterWindow* self);
    void enableFFTChannel(int ch);
    void disableFFTChannel(int ch);
    void stopWorker();
};

#endif // FFTPLOTTERWINDOW_H

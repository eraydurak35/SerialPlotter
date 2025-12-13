#ifndef FFTPLOTTERWINDOW_H
#define FFTPLOTTERWINDOW_H

#include "qcustomplot/qcustomplot.h"
#include <QWidget>
#include "fftworkermulti.h"
#include "datapacket.h"

namespace Ui {
class FFTPlotterWindow;
}

class FFTPlotterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FFTPlotterWindow(QWidget *parent = nullptr);
    ~FFTPlotterWindow();

    void setGraphIndex(int idx) {
        setWindowTitle(QString("FFT Graph %1").arg(idx));
    }

private:
    Ui::FFTPlotterWindow *ui;

    void closeEvent(QCloseEvent *event);
    void createSignalSelector(int count);
    void createDSPSignalSelector(int count);
    void addChannel(int channel);
    void addDSPChannel(int channel);
    void removeChannel(int channel);
    void removeDSPChannel(int channel);

    FFTWorkerMulti *fftworker = nullptr;
    QThread *fftworkerthread = nullptr;
    FFTWorkerMulti *fftworkerdsp = nullptr;
    QThread *fftworkerthreaddsp = nullptr;
    QMap<int, QCPGraph*> graphMap;
    QMap<int, QCPGraph*> DSPGraphMap;
    int max_channel_count = 0;
    int max_dsp_channel_count = 0;
    double xRange = 5.0;  // 5 saniyelik kaydırma
    double yRange = 1.0;
    double yOffset = 0.0;

    QMap<int, QCheckBox*> channelCheckboxes;
    QMap<int, QCheckBox*> DSPChannelCheckboxes;

    QMap<int, QColor> channelColors;

    QSet<int> selectedChannels;
    QSet<int> selectedDSPChannels;

    QMap<int, QCPItemTracer*> peakMarker;
    QMap<int, QCPItemText*>   peakLabel;
    QMap<int, QCPItemTracer*> DSPpeakMarker;
    QMap<int, QCPItemText*>   DSPpeakLabel;

public slots:
    void onNewData(DataPacket packet);
    void onNewDSPData(DataPacket packet);

    void onFFTReadyMulti(const QMap<int, QVector<double>> &freqs,
                         const QMap<int, QVector<double>> &mags);

    void onDSPFFTReadyMulti(const QMap<int, QVector<double>> &freqs,
                         const QMap<int, QVector<double>> &mags);

signals:
    void addNewFFTData(DataPacket packet);
    void addNewDSPFFTData(DataPacket packet);
    void graphClosed(FFTPlotterWindow* self);
    void enableFFTChannel(int ch);
    void disableFFTChannel(int ch);
    void enableDSPFFTChannel(int ch);
    void disableDSPFFTChannel(int ch);
    void stopWorker();
};

#endif // FFTPLOTTERWINDOW_H

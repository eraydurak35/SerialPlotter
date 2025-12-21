#ifndef PLOTTERWINDOW_H
#define PLOTTERWINDOW_H

#include "qcustomplot/qcustomplot.h"
#include "datapacket.h"
#include <QWidget>

namespace Ui {
class PlotterWindow;
}

class PlotterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlotterWindow(QWidget *parent = nullptr);
    ~PlotterWindow();

    void setGraphIndex(int idx) {
        setWindowTitle(QString("Graph %1").arg(idx));
    }

private:
    Ui::PlotterWindow *ui;

    void closeEvent(QCloseEvent *event);
    void createSignalSelector(int count);
    void createDSPSignalSelector(int count);
    void addChannel(int channel);
    void addDSPChannel(int channel);
    void removeChannel(int channel);
    void removeDSPChannel(int channel);
    void updateTimeLabelPosition(const QPoint &mousePos);
    QSize estimateLabelSize(QCPItemText *label);

    QMap<int, QCPGraph*> graphMap;
    QMap<int, QCPGraph*> dspGraphMap;
    int max_channel_count = 0;
    int max_dsp_channel_count = 0;
    double xRange = 5.0;
    double yRange = 3.0;
    double yOffset = 0.0;

    QCPItemLine   *timeLine = nullptr;
    QCPItemText   *timeLabel = nullptr;

    QCPItemText *pausedLabel = nullptr;

    QMap<int, QCheckBox*> channelCheckboxes;
    QMap<int, QCheckBox*> DSPChannelCheckboxes;
    QMap<int, QCPItemTracer*> tracers;
    QMap<int, QCPItemTracer*> DSPtracers;
    QMap<int, QColor> channelColors;
    bool isGraphPaused = false;

public slots:
    void onNewData(DataPacket packet);
    void onNewDSPData(DataPacket packet);

signals:

    void graphClosed(PlotterWindow* self);
private slots:
    void on_yoffsetspinbox_valueChanged(double arg1);
    void on_yrangespinbox_valueChanged(double arg1);
    void on_xrrangespinbox_valueChanged(double arg1);
    void onPlotMouseMove(QMouseEvent *event);
};

#endif // PLOTTERWINDOW_H

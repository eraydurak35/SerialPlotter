#ifndef PLOTTERWINDOW_H
#define PLOTTERWINDOW_H

#include "qcustomplot/qcustomplot.h"
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

    void addChannel(int channel);
    void setGraphIndex(int idx) {
        setWindowTitle(QString("Graph %1").arg(idx));
    }

private:
    Ui::PlotterWindow *ui;

    void closeEvent(QCloseEvent *event);
    void createSignalSelector(int count);
    void removeChannel(int channel);

    QMap<int, QCPGraph*> graphMap;
    int max_channel_count = 0;
    double xRange = 5.0;  // 5 saniyelik kaydırma
    double yRange = 1.0;
    double yOffset = 0.0;

    QMap<int, QCheckBox*> channelCheckboxes;

    QMap<int, QColor> channelColors;

public slots:
    void onNewData(int channel, qint64 timestamp, double value);

signals:

    void graphClosed(PlotterWindow* self);
private slots:
    void on_yoffsetspinbox_valueChanged(double arg1);
    void on_yrangespinbox_valueChanged(double arg1);
    void on_xrrangespinbox_valueChanged(double arg1);
};

#endif // PLOTTERWINDOW_H

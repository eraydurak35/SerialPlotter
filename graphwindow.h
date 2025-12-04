#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include "qcheckbox.h"
#include "qgridlayout.h"
#include <QObject>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QToolButton>
#include <QSpinBox>


class GraphWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWindow(QWidget *parent = nullptr);

    void addChannel(int channel);   // kullanıcının istediği kanal ekleniyor

private:
    int max_channel_count = 0;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    double xRange = 5.0;

    QMap<int, QLineSeries*> seriesMap;  // channel -> series
    qint64 startTime;

    QMap<int, QCheckBox*> channelCheckboxes;

    QToolButton *toggleButton;
    QWidget *selectorPanel;
    QGridLayout *selectorLayout;


    QToolButton *axisButton;
    QWidget *axisPanel;
    QGridLayout *axisLayout;

    // X/Y ayarları için kontrol kutuları / spinboxlar
    QSpinBox *xRangeSpin;
    QDoubleSpinBox *yMinSpin;
    QDoubleSpinBox *yMaxSpin;

    void closeEvent(QCloseEvent *event);
    void createSignalSelector(int signalCount);
    void removeChannel(int channel);
    void applyAxisSettings();

signals:

    void graphClosed(GraphWindow* self);

public slots:
    void onNewData(int channel, qint64 timestamp, double value);

};

#endif // GRAPHWINDOW_H

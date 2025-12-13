#include "plotterwindow.h"
#include "qcustomplot/qcustomplot.h"
#include "ui_plotterwindow.h"

PlotterWindow::PlotterWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlotterWindow) {
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    QList<QColor> palette = {
        QColor(0xff0000), // 0 - Red
        QColor(0x00ff00), // 1 - Green
        QColor(0x0000ff), // 2 - Blue
        QColor(0xffff00), // 3 - Yellow
        QColor(0xff00ff), // 4 - Magenta
        QColor(0x00ffff), // 5 - Cyan

        QColor(0x2ca02c), // 6 - Dark Green
        QColor(0xff7f0e), // 7 - Orange
        QColor(0x1f77b4), // 8 - Steel Blue
        QColor(0xd62728), // 9 - Dark Red

        QColor(0x9467bd), // 10 - Purple
        QColor(0x8c564b), // 11 - Brown
        QColor(0xe377c2), // 12 - Pink
        QColor(0x7f7f7f), // 13 - Gray

        QColor(0xbcbd22), // 14 - Olive
        QColor(0x17becf), // 15 - Teal

        QColor(0xffc107), // 16 - Amber
        QColor(0x03a9f4), // 17 - Light Blue
        QColor(0x4caf50), // 18 - Lime Green
        QColor(0x9c27b0)  // 19 - Deep Purple
    };

    for (int i = 0; i < palette.size(); i++)
        channelColors[i] = palette[i];


    ui->plot->yAxis->setRange(-yRange, yRange);

    ui->plot->setBackground(QColor(53,53,53));
    ui->plot->axisRect()->setBackground(QColor(53,53,53));

    ui->plot->xAxis->setBasePen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setBasePen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickPen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setTickPen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setTickLabelColor(QColor(0xFFFFFF));

    ui->plot->xAxis->setLabel("Time (s)");

    ui->plot->xAxis->setLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setLabelColor(QColor(0xFFFFFF));

    ui->plot->xAxis->setLabelFont(QFont("Arial", 8));
    ui->plot->yAxis->setLabelFont(QFont("Arial", 8));

    ui->plot->setInteractions(
        QCP::iRangeZoom //|      // Mouse wheel ile zoom
        // QCP::iRangeDrag        // Mouse drag ile pan
        );

}

PlotterWindow::~PlotterWindow() {
    delete ui;
}

void PlotterWindow::closeEvent(QCloseEvent *event) {

    emit graphClosed(this);
    QWidget::closeEvent(event);
}

void PlotterWindow::onNewData(DataPacket packet) {

    ui->dataFreq->setText(QString("%1 Hz").arg(QString::number(packet.data_frequency, 'f', 0)));

    for (int i = 0; i < packet.values.size(); i++) {

        if ((i + 1) > max_channel_count) {
            createSignalSelector(i + 1);
            max_channel_count = i + 1;
        }

        // Kanal yoksa (seçilmemişse) çık
        if (graphMap.contains(i)) {
            double tSec = packet.timestamp / 1000.0;

            QCPGraph *g = graphMap[i];

            // --- QCustomPlot'ta veri ekleme ---
            g->addData(tSec, packet.values[i]);

            // --- Kaydırmalı X ekseni ---
            ui->plot->xAxis->setRange(tSec - xRange, tSec);

            // --- Yeniden çizim ---
            ui->plot->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void PlotterWindow::onNewDSPData(DataPacket packet) {

    ui->dsp_data_freq->setText(QString("%1 Hz").arg(QString::number(packet.data_frequency, 'f', 0)));

    for (int i = 0; i < packet.values.size(); i++) {

        if ((i + 1) > max_dsp_channel_count) {
            createDSPSignalSelector(i + 1);
            max_dsp_channel_count = i + 1;
        }

        // Kanal yoksa (seçilmemişse) çık
        if (dspGraphMap.contains(i)) {
            double tSec = packet.timestamp / 1000.0;

            QCPGraph *g = dspGraphMap[i];

            // --- QCustomPlot'ta veri ekleme ---
            g->addData(tSec, packet.values[i]);

            // --- Kaydırmalı X ekseni ---
            ui->plot->xAxis->setRange(tSec - xRange, tSec);

            // --- Yeniden çizim ---
            ui->plot->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void PlotterWindow::addChannel(int channel) {
    if (graphMap.contains(channel))
        return;

    QCPGraph *g = ui->plot->addGraph();
    // Rengi ata
    QColor c = channelColors.contains(channel)
                   ? channelColors[channel]
                   : QColor::fromHsv((channel * 36) % 360, 255, 255);  // fallback

    g->setPen(QPen(c, 1));

    g->setName(QString("Ch %1").arg(channel));

    graphMap[channel] = g;
}

void PlotterWindow::addDSPChannel(int channel) {
    if (dspGraphMap.contains(channel))
        return;

    QCPGraph *g = ui->plot->addGraph();
    // Aynı renkler aynı channel ile eşleşmesin diye offset ekliyoruz
    QColor c = channelColors.contains(channel + max_channel_count)
                   ? channelColors[channel + max_channel_count]
                   : QColor::fromHsv((channel * 36) % 360, 255, 255);  // fallback

    g->setPen(QPen(c, 1));

    g->setName(QString("Ch %1").arg(channel));

    dspGraphMap[channel] = g;
}

void PlotterWindow::createSignalSelector(int count) {

    channelCheckboxes.clear();

    const int cols = 20;

    for (int ch = 0; ch < count; ++ch) {

        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));
        chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QFrame *colorBox = new QFrame();
        colorBox->setFixedSize(14, 14);
        colorBox->setFrameStyle(QFrame::Box | QFrame::Plain);

        QColor c = channelColors[ch];
        colorBox->setStyleSheet(QString("background-color: %1;").arg(c.name()));

        // Checkbox + renk kutusu için minik yatay layout
        QWidget *container = new QWidget();
        QHBoxLayout *h = new QHBoxLayout(container);
        h->setContentsMargins(0,0,0,0);
        h->setSpacing(4);

        h->addWidget(colorBox);
        h->addWidget(chk);

        int row = ch / cols;
        int col = ch % cols;

        ui->signals_tab_grid_layout->addWidget(container, row, col);

        connect(chk, &QCheckBox::toggled, this, [=](bool on){
            if (on) addChannel(ch);
            else    removeChannel(ch);
        });

        channelCheckboxes[ch] = chk;
    }
}

void PlotterWindow::createDSPSignalSelector(int count) {

    DSPChannelCheckboxes.clear();

    const int cols = 20;

    for (int ch = 0; ch < count; ++ch) {

        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));
        chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QFrame *colorBox = new QFrame();
        colorBox->setFixedSize(14, 14);
        colorBox->setFrameStyle(QFrame::Box | QFrame::Plain);

        QColor c = channelColors[ch + count];
        colorBox->setStyleSheet(QString("background-color: %1;").arg(c.name()));

        // Checkbox + renk kutusu için minik yatay layout
        QWidget *container = new QWidget();
        QHBoxLayout *h = new QHBoxLayout(container);
        h->setContentsMargins(0,0,0,0);
        h->setSpacing(4);

        h->addWidget(colorBox);
        h->addWidget(chk);

        int row = ch / cols;
        int col = ch % cols;

        ui->dsp_signals_layout->addWidget(container, row, col);

        connect(chk, &QCheckBox::toggled, this, [=](bool on){
            if (on) addDSPChannel(ch);
            else    removeDSPChannel(ch);
        });

        DSPChannelCheckboxes[ch] = chk;
    }
}

void PlotterWindow::removeChannel(int channel) {
    if (!graphMap.contains(channel))
        return;
    ui->plot->removeGraph(graphMap[channel]);
    graphMap.remove(channel);
}

void PlotterWindow::removeDSPChannel(int channel) {
    if (!dspGraphMap.contains(channel))
        return;
    ui->plot->removeGraph(dspGraphMap[channel]);
    dspGraphMap.remove(channel);
}

void PlotterWindow::on_yoffsetspinbox_valueChanged(double arg1) {
    yOffset = arg1;
    ui->plot->yAxis->setRange(-yRange + arg1, yRange + arg1);
}

void PlotterWindow::on_yrangespinbox_valueChanged(double arg1) {
    yRange = arg1;
    ui->plot->yAxis->setRange(-yRange + yOffset, yRange + yOffset);
}

void PlotterWindow::on_xrrangespinbox_valueChanged(double arg1) {
    xRange = arg1;
}


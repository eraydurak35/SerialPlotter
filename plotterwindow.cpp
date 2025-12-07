#include "plotterwindow.h"
#include "qcustomplot/qcustomplot.h"
#include "ui_plotterwindow.h"

PlotterWindow::PlotterWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlotterWindow) {
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    QList<QColor> palette = {
        QColor(0xff0000), // 0 - kırmızı
        QColor(0x00ff00), // 1 - yeşil
        QColor(0x0000ff), // 2 - mavi
        QColor(0xffff00), // 3 - sarı
        QColor(0xff00ff), // 4 - magenta
        QColor(0x00ffff), // 5 - cyan
        QColor(0xffaa00),
        QColor(0x00ffaa),
        QColor(0xaa00ff),
        QColor(0xffffff)
    };

    for (int i = 0; i < palette.size(); i++)
        channelColors[i] = palette[i];


    ui->plot->yAxis->setRange(-yRange, yRange);

    ui->plot->setBackground(QColor(0x121212));
    ui->plot->axisRect()->setBackground(QColor(0x1E1E1E));

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

    static uint32_t start_timestamp = 0;
    static uint32_t counter = 0;

    if (counter++ == 0) {
        start_timestamp = packet.timestamp;
    }
    else if (counter++ >= 1000) {
        uint32_t newDataFrequency = 500000 / (packet.timestamp - start_timestamp);
        ui->dataFreq->setText(QString("%1 Hz").arg(newDataFrequency));
        counter = 0;
    }

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

void PlotterWindow::createSignalSelector(int count) {

    channelCheckboxes.clear();

    const int cols = 10;

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

void PlotterWindow::removeChannel(int channel) {
    if (!graphMap.contains(channel))
        return;
    ui->plot->removeGraph(graphMap[channel]);
    graphMap.remove(channel);
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


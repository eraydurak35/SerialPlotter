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

    pausedLabel = new QCPItemText(ui->plot);
    pausedLabel->setLayer("overlay");
    pausedLabel->setText("PAUSED");
    pausedLabel->setFont(QFont("Consolas", 11, QFont::Bold));
    pausedLabel->setColor(Qt::white);
    pausedLabel->setPen(QPen(Qt::white));
    pausedLabel->setBrush(QBrush(QColor(0, 0, 0, 120)));
    pausedLabel->setPadding(QMargins(6, 3, 6, 3));
    pausedLabel->position->setType(QCPItemPosition::ptAbsolute);
    pausedLabel->position->setCoords(ui->plot->width() / 2, 15);
    pausedLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignTop);
    pausedLabel->setVisible(false);

    ui->plot->setInteractions(
        QCP::iRangeZoom |      // Mouse wheel ile zoom
        QCP::iRangeDrag        // Mouse drag ile pan
        );

    QShortcut *spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);

    connect(spaceShortcut, &QShortcut::activated, this, [this](){
        isGraphPaused = !isGraphPaused;

        pausedLabel->setVisible(isGraphPaused);

        for (int i = 0; i < channelCheckboxes.size(); i++) {
            channelCheckboxes[i]->setEnabled(!isGraphPaused);
        }
        for (int i = 0; i < DSPChannelCheckboxes.size(); i++) {
            DSPChannelCheckboxes[i]->setEnabled(!isGraphPaused);
        }

        if (pausedLabel) {
            pausedLabel->position->setCoords(ui->plot->width() / 2, 15);
        }
    });

    connect(ui->plot, &QCustomPlot::mouseMove, this, &PlotterWindow::onPlotMouseMove);

    timeLine = new QCPItemLine(ui->plot);
    timeLine->setPen(QPen(Qt::DashLine));
    timeLine->setLayer("overlay");

    timeLine->start->setType(QCPItemPosition::ptPlotCoords);
    timeLine->end->setType(QCPItemPosition::ptPlotCoords);

    timeLabel = new QCPItemText(ui->plot);
    timeLabel->position->setType(QCPItemPosition::ptAbsolute);
    timeLabel->setLayer("overlay");
    timeLabel->setPadding(QMargins(4, 2, 4, 2));
    timeLabel->setBrush(QBrush(QColor(30, 30, 30, 180)));
    timeLabel->setPen(QPen(Qt::white));
    timeLabel->setColor(Qt::white);
    timeLabel->setFont(QFont("Consolas", 9));

}

PlotterWindow::~PlotterWindow() {
    delete ui;
}

static bool graphHasData(QCPGraph *g)
{
    return g &&
           g->data() &&
           !g->data()->isEmpty();
}

QSize PlotterWindow::estimateLabelSize(QCPItemText *label)
{
    QFontMetrics fm(label->font());

    QRect r = fm.boundingRect(
        QRect(0, 0, 1000, 1000),
        Qt::TextWordWrap,
        label->text()
        );

    QSize s = r.size();
    s.rwidth()  += label->padding().left() + label->padding().right();
    s.rheight() += label->padding().top()  + label->padding().bottom();

    return s;
}

void PlotterWindow::updateTimeLabelPosition(const QPoint &mousePos)
{

    auto *plot = ui->plot;

    // AxisRect sınırlarını PLOT koordinatına al
    QRect axisRect = plot->axisRect()->rect();
    axisRect.translate(plot->axisRect()->topLeft());

    QSize labelSize = estimateLabelSize(timeLabel);
    const int margin = 12;

    int x;
    int y;

    // Varsayılan: SAĞ
    if (mousePos.x() + margin + labelSize.width() <= axisRect.right()) {
        x = mousePos.x() + margin;
    } else {
        x = mousePos.x() - labelSize.width() - margin;
    }

    // Varsayılan: ALT
    if (mousePos.y() + margin + labelSize.height() <= axisRect.bottom()) {
        y = mousePos.y() + margin;
    } else {
        y = mousePos.y() - labelSize.height() - margin;
    }

    // 🔒 SOL / ÜST clamp (ARTIK ÇALIŞIR)
    if (x < axisRect.left())
        x = axisRect.left() + margin;

    if (y < axisRect.top())
        y = axisRect.top() + margin;

    timeLabel->position->setCoords(x, y);
}

void PlotterWindow::onPlotMouseMove(QMouseEvent *event)
{

    if (!ui->plot->axisRect()->rect().contains(event->pos())) {
        timeLine->setVisible(false);
        timeLabel->setVisible(false);

        for (auto tr : tracers)
            tr->setVisible(false);
        for (auto tr : DSPtracers)
            tr->setVisible(false);

        ui->plot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    auto *plot = ui->plot;

    if (!plot->axisRect()->rect().contains(event->pos()))
        return;

    // En az bir grafik olmalı (normal veya DSP)
    if (plot->graphCount() == 0)
        return;

    // Mouse → X
    double x = plot->xAxis->pixelToCoord(event->pos().x());

    // Referans grafik: önce normal, yoksa DSP
    QCPGraph *refGraph = nullptr;

    if (!graphMap.isEmpty())
        refGraph = graphMap.first();
    else if (!dspGraphMap.isEmpty())
        refGraph = dspGraphMap.first();
    else
        return;

    if (refGraph->data()->isEmpty())
        return;

    // SNAP
    auto it = refGraph->data()->findBegin(x, true);
    if (it == refGraph->data()->constEnd())
        return;

    double snappedX = it->key;

    // -----------------------------
    // Dikey zaman çizgisi
    // -----------------------------
    double yMin = plot->yAxis->range().lower;
    double yMax = plot->yAxis->range().upper;

    timeLine->start->setCoords(snappedX, yMin);
    timeLine->end->setCoords(snappedX, yMax);
    timeLine->setVisible(true);

    // -----------------------------
    // LABEL TEXT
    // -----------------------------
    QString text;
    text += QString("t = %1 s\n").arg(snappedX, 0, 'f', 3);

    // =====================================================
    // NORMAL CHANNELS
    // =====================================================
    for (auto it = graphMap.begin(); it != graphMap.end(); ++it) {

        int ch = it.key();
        QCPGraph *g = it.value();

        if (!g || g->data()->isEmpty())
            continue;

        auto git = g->data()->findBegin(snappedX);
        if (git == g->data()->constEnd())
            continue;

        double y = git->value;

        // Tracer
        if (tracers.contains(ch)) {
            QCPItemTracer *tr = tracers[ch];
            if (graphHasData(g)) {
                tr->setGraphKey(snappedX);
                tr->updatePosition();
                tr->setVisible(true);
            } else {
                tr->setVisible(false);
            }
        }

        text += QString("Ch %1 : %2\n")
                    .arg(ch)
                    .arg(y, 0, 'f', 3);
    }

    // =====================================================
    // DSP CHANNELS
    // =====================================================
    for (auto it = dspGraphMap.begin(); it != dspGraphMap.end(); ++it) {

        int ch = it.key();
        QCPGraph *g = it.value();

        if (!g || g->data()->isEmpty())
            continue;

        auto git = g->data()->findBegin(snappedX);
        if (git == g->data()->constEnd())
            continue;

        double y = git->value;

        // DSP Tracer
        if (DSPtracers.contains(ch)) {
            QCPItemTracer *tr = DSPtracers[ch];
            if (graphHasData(g)) {
                tr->setGraphKey(snappedX);
                tr->updatePosition();
                tr->setVisible(true);
            } else {
                tr->setVisible(false);
            }
        }

        text += QString("DSP %1 : %2\n")
                    .arg(ch)
                    .arg(y, 0, 'f', 3);
    }

    // -----------------------------
    // LABEL (pixel space)
    // -----------------------------
    timeLabel->setText(text.trimmed());
    updateTimeLabelPosition(event->pos());
    timeLabel->setVisible(true);

    plot->replot(QCustomPlot::rpQueuedReplot);

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
        if (graphMap.contains(i) & !isGraphPaused) {
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
        if (dspGraphMap.contains(i) && !isGraphPaused) {
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

    QCPItemTracer *tr = tracers[channel];
    tr->setGraph(g);
    tr->setGraphKey(0);
    tr->setVisible(false);
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

    QCPItemTracer *tr = DSPtracers[channel];
    tr->setGraph(g);
    tr->setGraphKey(0);
    tr->setVisible(false);
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


    auto *plot = ui->plot;
    // Eski tracer'ları temizle
    for (auto tr : tracers) {
        plot->removeItem(tr);
    }
    tracers.clear();

    for (int ch = 0; ch < count; ++ch) {

        QCPItemTracer *tr = new QCPItemTracer(plot);

        tr->setStyle(QCPItemTracer::tsCircle);
        tr->setSize(7);
        tr->setInterpolating(false);   // SNAP
        tr->setLayer("overlay");
        tr->setVisible(false);

        QColor c = channelColors[ch];
        tr->setPen(QPen(c));
        tr->setBrush(c);

        // 🔑 MAP'e ekle
        tracers.insert(ch, tr);
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


    auto *plot = ui->plot;
    // Eski tracer'ları temizle
    for (auto tr : tracers) {
        plot->removeItem(tr);
    }
    DSPtracers.clear();

    for (int ch = 0; ch < count; ++ch) {

        QCPItemTracer *tr = new QCPItemTracer(plot);

        tr->setStyle(QCPItemTracer::tsCircle);
        tr->setSize(7);
        tr->setInterpolating(false);   // SNAP
        tr->setLayer("overlay");
        tr->setVisible(false);

        QColor c = channelColors[ch + count];
        tr->setPen(QPen(c));
        tr->setBrush(c);

        // 🔑 MAP'e ekle
        DSPtracers.insert(ch, tr);
    }
}

void PlotterWindow::removeChannel(int channel) {
    if (!graphMap.contains(channel))
        return;
    ui->plot->removeGraph(graphMap[channel]);
    graphMap.remove(channel);

    if (tracers[channel]) {
        tracers[channel]->setVisible(false);
        tracers[channel]->setGraph(nullptr);
    }

    ui->plot->replot();
}

void PlotterWindow::removeDSPChannel(int channel) {
    if (!dspGraphMap.contains(channel))
        return;
    ui->plot->removeGraph(dspGraphMap[channel]);
    dspGraphMap.remove(channel);

    if (DSPtracers[channel]) {
        DSPtracers[channel]->setVisible(false);
        DSPtracers[channel]->setGraph(nullptr);
    }

    ui->plot->replot();
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


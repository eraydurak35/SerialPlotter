#include "fftplotterwindow.h"
#include "ui_fftplotterwindow.h"
#include <QThread>
#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QPen>
#include <utility>      // std::as_const

FFTPlotterWindow::FFTPlotterWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FFTPlotterWindow) {

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    // --------------------------------------------------
    //  RENK PALETİ
    // --------------------------------------------------
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

    for (int i = 0; i < palette.size(); ++i)
        channelColors[i] = palette[i];

    // --------------------------------------------------
    //  PLOT TEMA (FFT İÇİN)
    // --------------------------------------------------
    ui->plot->setBackground(QColor(53,53,53));
    ui->plot->axisRect()->setBackground(QColor(53,53,53));

    ui->plot->xAxis->setBasePen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setBasePen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickPen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setTickPen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setTickLabelColor(QColor(0xFFFFFF));

    ui->plot->yAxis->setRange(-120, 0);      // dB ekseni
    ui->plot->xAxis->setLabel("Frequency (Hz)");
    ui->plot->yAxis->setLabel("Magnitude (dB)");

    ui->plot->xAxis->setLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setLabelColor(QColor(0xFFFFFF));

    ui->plot->xAxis->setLabelFont(QFont("Arial", 8));
    ui->plot->yAxis->setLabelFont(QFont("Arial", 8));

    ui->plot->legend->setVisible(true);
    ui->plot->legend->setBrush(QColor(0x101010));
    ui->plot->legend->setTextColor(Qt::white);

    // --------------------------------------------------
    //  FFT WORKER + THREAD
    // --------------------------------------------------
    fftworkerthread = new QThread(this);
    fftworker       = new FFTWorkerMulti();
    fftworker->moveToThread(fftworkerthread);

    // UI -> Worker bağlantıları
    connect(this, &FFTPlotterWindow::addNewFFTData,
            fftworker, &FFTWorkerMulti::addSample);

    connect(this, &FFTPlotterWindow::enableFFTChannel,
            fftworker, &FFTWorkerMulti::enableChannel);

    connect(this, &FFTPlotterWindow::disableFFTChannel,
            fftworker, &FFTWorkerMulti::disableChannel);

    connect(this, &FFTPlotterWindow::stopWorker,
            fftworker, &FFTWorkerMulti::stop);

    // Worker -> UI bağlantısı
    connect(fftworker, &FFTWorkerMulti::fftReadyMulti,
            this, &FFTPlotterWindow::onFFTReadyMulti);

    fftworkerthread->start();


    fftworkerthreaddsp = new QThread(this);
    fftworkerdsp       = new FFTWorkerMulti();

    fftworkerdsp->moveToThread(fftworkerthreaddsp);
    // UI -> Worker bağlantıları
    connect(this, &FFTPlotterWindow::addNewDSPFFTData,
            fftworkerdsp, &FFTWorkerMulti::addSample);

    connect(this, &FFTPlotterWindow::enableDSPFFTChannel,
            fftworkerdsp, &FFTWorkerMulti::enableChannel);

    connect(this, &FFTPlotterWindow::disableDSPFFTChannel,
            fftworkerdsp, &FFTWorkerMulti::disableChannel);

    connect(this, &FFTPlotterWindow::stopWorker,
            fftworkerdsp, &FFTWorkerMulti::stop);

    // Worker -> UI bağlantısı
    connect(fftworkerdsp, &FFTWorkerMulti::fftReadyMulti,
            this, &FFTPlotterWindow::onDSPFFTReadyMulti);

    fftworkerthreaddsp->start();
}

FFTPlotterWindow::~FFTPlotterWindow() {
    // Worker'a dur sinyali
    emit stopWorker();

    if (fftworkerthread && fftworkerthread->isRunning()) {
        fftworkerthread->quit();
        fftworkerthread->wait();
    }

    if (fftworkerthreaddsp && fftworkerthreaddsp->isRunning()) {
        fftworkerthreaddsp->quit();
        fftworkerthreaddsp->wait();
    }

    delete fftworker;
    delete fftworkerdsp;
    delete ui;
}

void FFTPlotterWindow::closeEvent(QCloseEvent *event) {
    emit graphClosed(this);
    QWidget::closeEvent(event);
}

// ----------------------------------------------------------------------
//  SERIAL / PLOTTER TARAFINDAN ÇAĞRILAN SLOT
//  Ham zaman-domain veriyi direkt FFT worker'a forward ediyoruz
// ----------------------------------------------------------------------
void FFTPlotterWindow::onNewData(DataPacket packet) {

    ui->dataFreq->setText(QString("%1 Hz").arg(QString::number(packet.data_frequency, 'f', 0)));

    // Gerekiyorsa yeni kanal ekle
    for (int i = 0; i < packet.values.size(); i++) {
        if ((i + 1) > max_channel_count) {
            createSignalSelector(i + 1);
            max_channel_count = i + 1;
        }
    }

    emit addNewFFTData(packet);
}

void FFTPlotterWindow::onNewDSPData(DataPacket packet) {

    ui->dspDataFreq->setText(QString("%1 Hz").arg(QString::number(packet.data_frequency, 'f', 0)));

    // Gerekiyorsa yeni kanal ekle
    for (int i = 0; i < packet.values.size(); i++) {
        if ((i + 1) > max_dsp_channel_count) {
            createDSPSignalSelector(i + 1);
            max_dsp_channel_count = i + 1;
        }
    }

    emit addNewDSPFFTData(packet);
}

// ----------------------------------------------------------------------
//  FFT PENCERESİNDE GÖSTERİLECEK KANALA GRAFİK EKLE
// ----------------------------------------------------------------------
void FFTPlotterWindow::addChannel(int channel) {
    if (graphMap.contains(channel))
        return;

    QCPGraph *g = ui->plot->addGraph();

    QColor c = channelColors.contains(channel)
                   ? channelColors[channel]
                   : QColor::fromHsv((channel * 36) % 360, 255, 255);  // fallback

    g->setPen(QPen(c, 1));
    g->setAntialiased(false);
    g->setName(QString("Ch %1").arg(channel));

    graphMap[channel] = g;
}

void FFTPlotterWindow::addDSPChannel(int channel) {
    if (DSPGraphMap.contains(channel))
        return;

    QCPGraph *g = ui->plot->addGraph();

    QColor c = channelColors.contains(channel + max_dsp_channel_count)
                   ? channelColors[channel + max_dsp_channel_count]
                   : QColor::fromHsv((channel * 36) % 360, 255, 255);  // fallback

    g->setPen(QPen(c, 1));
    g->setAntialiased(false);
    g->setName(QString("Ch %1").arg(channel));

    DSPGraphMap[channel] = g;
}

// ----------------------------------------------------------------------
//  SİNYAL SEÇME PANELİ (CHECKBOX + RENK KUTUSU)
// ----------------------------------------------------------------------
void FFTPlotterWindow::createSignalSelector(int count) {
    channelCheckboxes.clear();

    const int cols = 10;

    for (int ch = 0; ch < count; ++ch)
    {
        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));
        chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QFrame *colorBox = new QFrame();
        colorBox->setFixedSize(14, 14);
        colorBox->setFrameStyle(QFrame::Box | QFrame::Plain);

        QColor c = channelColors.value(ch, QColor::fromHsv((ch * 36) % 360, 255, 255));
        colorBox->setStyleSheet(QString("background-color: %1;").arg(c.name()));

        QWidget *container = new QWidget();
        QHBoxLayout *h = new QHBoxLayout(container);
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(4);
        h->addWidget(colorBox);
        h->addWidget(chk);

        int row = ch / cols;
        int col = ch % cols;

        ui->signals_tab_grid_layout->addWidget(container, row, col);

        connect(chk, &QCheckBox::toggled, this, [=](bool on){
            if (on) {
                addChannel(ch);
                selectedChannels.insert(ch);
                emit enableFFTChannel(ch);    // worker'da bu kanal aktifleştir
            } else {
                removeChannel(ch);
                selectedChannels.remove(ch);
                emit disableFFTChannel(ch);   // worker'da bu kanal pasifleştir
            }
        });

        channelCheckboxes[ch] = chk;
    }
}


void FFTPlotterWindow::createDSPSignalSelector(int count) {
    DSPChannelCheckboxes.clear();

    const int cols = 10;

    for (int ch = 0; ch < count; ++ch)
    {
        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));
        chk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QFrame *colorBox = new QFrame();
        colorBox->setFixedSize(14, 14);
        colorBox->setFrameStyle(QFrame::Box | QFrame::Plain);

        QColor c = channelColors.contains(ch + max_dsp_channel_count + 1)
                       ? channelColors[ch + max_dsp_channel_count + 1]
                       : QColor::fromHsv((ch * 36) % 360, 255, 255);  // fallback

        colorBox->setStyleSheet(QString("background-color: %1;").arg(c.name()));

        QWidget *container = new QWidget();
        QHBoxLayout *h = new QHBoxLayout(container);
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(4);
        h->addWidget(colorBox);
        h->addWidget(chk);

        int row = ch / cols;
        int col = ch % cols;

        ui->dspSignalsTabGridLayout->addWidget(container, row, col);

        connect(chk, &QCheckBox::toggled, this, [=](bool on){
            if (on) {
                addDSPChannel(ch);
                selectedDSPChannels.insert(ch);
                emit enableDSPFFTChannel(ch);    // worker'da bu kanal aktifleştir
            } else {
                removeDSPChannel(ch);
                selectedDSPChannels.remove(ch);
                emit disableDSPFFTChannel(ch);   // worker'da bu kanal pasifleştir
            }
        });

        DSPChannelCheckboxes[ch] = chk;
    }
}

// ----------------------------------------------------------------------
//  GRAFİKTEN KANALI KALDIR
// ----------------------------------------------------------------------
void FFTPlotterWindow::removeChannel(int channel) {

    if (graphMap.contains(channel)) {
        ui->plot->removeGraph(graphMap[channel]);
        graphMap.remove(channel);
    }

    // --- PEAK TRACER ---
    if (peakMarker.contains(channel))
    {
        // ui->plot->removeItem(peakMarker[channel]); // ÖNEMLİ!
        delete peakMarker[channel];
        peakMarker.remove(channel);
    }

    // --- PEAK LABEL ---
    if (peakLabel.contains(channel))
    {
        // ui->plot->removeItem(peakLabel[channel]); // ÖNEMLİ!
        delete peakLabel[channel];
        peakLabel.remove(channel);
    }

    ui->plot->replot(QCustomPlot::rpQueuedReplot);
}

void FFTPlotterWindow::removeDSPChannel(int channel) {

    if (DSPGraphMap.contains(channel)) {
        ui->plot->removeGraph(DSPGraphMap[channel]);
        DSPGraphMap.remove(channel);
    }

    // --- PEAK TRACER ---
    if (DSPpeakMarker.contains(channel))
    {
        // ui->plot->removeItem(peakMarker[channel]); // ÖNEMLİ!
        delete DSPpeakMarker[channel];
        DSPpeakMarker.remove(channel);
    }

    // --- PEAK LABEL ---
    if (DSPpeakLabel.contains(channel))
    {
        // ui->plot->removeItem(peakLabel[channel]); // ÖNEMLİ!
        delete DSPpeakLabel[channel];
        DSPpeakLabel.remove(channel);
    }

    ui->plot->replot(QCustomPlot::rpQueuedReplot);
}

// ----------------------------------------------------------------------
//  WORKER'DAN GELEN ÇOKLU FFT SONUCUNU ÇİZ
// ----------------------------------------------------------------------
void FFTPlotterWindow::onFFTReadyMulti(const QMap<int, QVector<double>> &freqs,
                                       const QMap<int, QVector<double>> &mags) {
    if (freqs.isEmpty())
        return;

    // Seçili her kanal için FFT'yi güncelle
    for (int ch : std::as_const(selectedChannels)) {
        if (!freqs.contains(ch))
            continue;

        const QVector<double> &fx = freqs[ch];
        const QVector<double> &my = mags[ch];

        if (fx.isEmpty() || my.isEmpty())
            continue;

        QCPGraph *g = nullptr;
        if (!graphMap.contains(ch)) {
            g = ui->plot->addGraph();
            QColor c = channelColors.value(ch, QColor::fromHsv((ch * 36) % 360, 255, 255));
            g->setPen(QPen(c, 1));
            g->setAntialiased(false);
            g->setName(QString("Ch %1").arg(ch));
            graphMap[ch] = g;
        } else {
            g = graphMap[ch];
        }

        // freq ve mag zaten sıralı → "already sorted" = true
        g->setData(fx, my, true);

        // ---------------------------------------
        //   PEAK FREKANS HESABI
        // ---------------------------------------
        int peakIndex = 0;
        double peakMag = -1e9;

        for (int i = 0; i < my.size(); i++) {
            if (my[i] > peakMag) {
                peakMag = my[i];
                peakIndex = i;
            }
        }

        double peakFreq = fx[peakIndex];

        graphMap[ch]->setName(QString("Peak: %2 Hz").arg(peakFreq, 0, 'f', 1));


        // ---------------------------
        // PEAK MARKER OLUŞTUR / GÜNCELLE
        // ---------------------------
        if (!peakMarker.contains(ch)) {
            // Tracer (nokta)
            QCPItemTracer *tr = new QCPItemTracer(ui->plot);
            tr->setGraph(graphMap[ch]);              // hangi grafiğe bağlı
            tr->setInterpolating(true);
            tr->setStyle(QCPItemTracer::tsCircle);
            tr->setPen(QPen(channelColors[ch], 2));
            tr->setBrush(QBrush(Qt::white));
            tr->setSize(6);

            peakMarker[ch] = tr;

            // Text etiketi
            QCPItemText *txt = new QCPItemText(ui->plot);
            txt->setColor(Qt::white);
            txt->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
            txt->position->setType(QCPItemPosition::ptPlotCoords);

            peakLabel[ch] = txt;
        }

        // Tracer: sadece X (key) veriyoruz, Y’i grafikten alıyor
        peakMarker[ch]->setGraph(graphMap[ch]);   // emin olmak için
        peakMarker[ch]->setGraphKey(peakFreq);

        // Etiket: Plot koordinatları ile direkt konum veriyoruz
        peakLabel[ch]->position->setCoords(peakFreq, peakMag);
        peakLabel[ch]->setText(QString("%1 Hz").arg(peakFreq, 0, 'f', 1));
    }

    // X ekseni: ilk kanalın frekans aralığına göre ayarla
    int firstCh = freqs.firstKey();
    const QVector<double> &fx0 = freqs[firstCh];
    if (!fx0.isEmpty()) {
        ui->plot->xAxis->setRange(0, fx0.constLast());
    }

    ui->plot->yAxis->setRange(-120, 0);
    ui->plot->replot(QCustomPlot::rpQueuedReplot);

}


void FFTPlotterWindow::onDSPFFTReadyMulti(const QMap<int, QVector<double>> &freqs,
                                       const QMap<int, QVector<double>> &mags) {
    if (freqs.isEmpty())
        return;

    // Seçili her kanal için FFT'yi güncelle
    for (int ch : std::as_const(selectedDSPChannels)) {
        if (!freqs.contains(ch))
            continue;

        const QVector<double> &fx = freqs[ch];
        const QVector<double> &my = mags[ch];

        if (fx.isEmpty() || my.isEmpty())
            continue;

        QCPGraph *g = nullptr;
        if (!DSPGraphMap.contains(ch)) {
            g = ui->plot->addGraph();
            QColor c = channelColors.value(ch + max_dsp_channel_count, QColor::fromHsv((ch * 36) % 360, 255, 255));
            g->setPen(QPen(c, 1));
            g->setAntialiased(false);
            g->setName(QString("Ch %1").arg(ch));
            DSPGraphMap[ch] = g;
        } else {
            g = DSPGraphMap[ch];
        }

        // freq ve mag zaten sıralı → "already sorted" = true
        g->setData(fx, my, true);

        // ---------------------------------------
        //   PEAK FREKANS HESABI
        // ---------------------------------------
        int peakIndex = 0;
        double peakMag = -1e9;

        for (int i = 0; i < my.size(); i++) {
            if (my[i] > peakMag) {
                peakMag = my[i];
                peakIndex = i;
            }
        }

        double peakFreq = fx[peakIndex];

        DSPGraphMap[ch]->setName(QString("Peak: %2 Hz").arg(peakFreq, 0, 'f', 1));


        // ---------------------------
        // PEAK MARKER OLUŞTUR / GÜNCELLE
        // ---------------------------
        if (!DSPpeakMarker.contains(ch)) {
            // Tracer (nokta)
            QCPItemTracer *tr = new QCPItemTracer(ui->plot);
            tr->setGraph(DSPGraphMap[ch]);              // hangi grafiğe bağlı
            tr->setInterpolating(true);
            tr->setStyle(QCPItemTracer::tsCircle);
            tr->setPen(QPen(channelColors[ch + max_dsp_channel_count], 2));
            tr->setBrush(QBrush(Qt::white));
            tr->setSize(6);

            DSPpeakMarker[ch] = tr;

            // Text etiketi
            QCPItemText *txt = new QCPItemText(ui->plot);
            txt->setColor(Qt::white);
            txt->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
            txt->position->setType(QCPItemPosition::ptPlotCoords);

            DSPpeakLabel[ch] = txt;
        }

        // Tracer: sadece X (key) veriyoruz, Y’i grafikten alıyor
        DSPpeakMarker[ch]->setGraph(DSPGraphMap[ch]);   // emin olmak için
        DSPpeakMarker[ch]->setGraphKey(peakFreq);

        // Etiket: Plot koordinatları ile direkt konum veriyoruz
        DSPpeakLabel[ch]->position->setCoords(peakFreq, peakMag);
        DSPpeakLabel[ch]->setText(QString("%1 Hz").arg(peakFreq, 0, 'f', 1));
    }

    // X ekseni: ilk kanalın frekans aralığına göre ayarla
    int firstCh = freqs.firstKey();
    const QVector<double> &fx0 = freqs[firstCh];
    if (!fx0.isEmpty()) {
        ui->plot->xAxis->setRange(0, fx0.constLast());
    }

    ui->plot->yAxis->setRange(-120, 0);
    ui->plot->replot(QCustomPlot::rpQueuedReplot);

}

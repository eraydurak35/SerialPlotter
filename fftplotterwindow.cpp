#include "fftplotterwindow.h"
#include "ui_fftplotterwindow.h"




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
    , ui(new Ui::FFTPlotterWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    // --------------------------------------------------
    //  RENK PALETİ
    // --------------------------------------------------
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

    for (int i = 0; i < palette.size(); ++i)
        channelColors[i] = palette[i];

    // --------------------------------------------------
    //  PLOT TEMA (FFT İÇİN)
    // --------------------------------------------------
    ui->plot->setBackground(QColor(0x121212));
    ui->plot->axisRect()->setBackground(QColor(0x1E1E1E));

    ui->plot->xAxis->setBasePen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setBasePen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickPen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setTickPen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setTickLabelColor(QColor(0xFFFFFF));

    ui->plot->yAxis->setRange(-120, 0);      // dB ekseni
    ui->plot->xAxis->setLabel("Frequency (Hz)");
    ui->plot->yAxis->setLabel("Magnitude (dB)");

    ui->plot->legend->setVisible(true);
    ui->plot->legend->setBrush(QColor(0x101010));
    ui->plot->legend->setTextColor(Qt::white);

    // --------------------------------------------------
    //  FFT WORKER + THREAD
    // --------------------------------------------------
    fftworkerthread = new QThread(this);
    fftworker       = new FFTWorkerMulti();   // windowSize / sampleRate worker içinde ayarlanmalı

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
            this, &FFTPlotterWindow::onFftReadyMulti);

    fftworkerthread->start();
}

FFTPlotterWindow::~FFTPlotterWindow()
{
    // Worker'a dur sinyali
    emit stopWorker();

    if (fftworkerthread && fftworkerthread->isRunning()) {
        fftworkerthread->quit();
        fftworkerthread->wait();
    }

    delete fftworker;   // moveToThread ile parent yoksa manuel silinir
    delete ui;
}

void FFTPlotterWindow::closeEvent(QCloseEvent *event)
{
    emit graphClosed(this);
    QWidget::closeEvent(event);
}

// ----------------------------------------------------------------------
//  SERIAL / PLOTTER TARAFINDAN ÇAĞRILAN SLOT
//  Ham zaman-domain veriyi direkt FFT worker'a forward ediyoruz
// ----------------------------------------------------------------------
void FFTPlotterWindow::onNewData(int channel, qint64 timestamp, double value)
{

    // Gerekiyorsa yeni kanal ekle
    if ((channel + 1) > max_channel_count) {
        createSignalSelector(channel + 1);
        max_channel_count = channel + 1;
    }

    emit addNewFFTData(channel, timestamp, value);
}

// ----------------------------------------------------------------------
//  FFT PENCERESİNDE GÖSTERİLECEK KANALA GRAFİK EKLE
// ----------------------------------------------------------------------
void FFTPlotterWindow::addChannel(int channel)
{
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

// ----------------------------------------------------------------------
//  SİNYAL SEÇME PANELİ (CHECKBOX + RENK KUTUSU)
// ----------------------------------------------------------------------
void FFTPlotterWindow::createSignalSelector(int count)
{
    channelCheckboxes.clear();

    const int cols = 10;

    for (int ch = 0; ch < count; ++ch)
    {
        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));

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

// ----------------------------------------------------------------------
//  GRAFİKTEN KANALI KALDIR
// ----------------------------------------------------------------------
void FFTPlotterWindow::removeChannel(int channel)
{
    if (!graphMap.contains(channel))
        return;

    ui->plot->removeGraph(graphMap[channel]);
    graphMap.remove(channel);
}

// ----------------------------------------------------------------------
//  WORKER'DAN GELEN ÇOKLU FFT SONUCUNU ÇİZ
// ----------------------------------------------------------------------
void FFTPlotterWindow::onFftReadyMulti(const QMap<int, QVector<double>> &freqs,
                                       const QMap<int, QVector<double>> &mags)
{
    if (freqs.isEmpty())
        return;

    // Seçili her kanal için FFT'yi güncelle
    for (int ch : std::as_const(selectedChannels))
    {
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

        for (int i = 0; i < my.size(); i++)
        {
            if (my[i] > peakMag)
            {
                peakMag = my[i];
                peakIndex = i;
            }
        }

        double peakFreq = fx[peakIndex];

        graphMap[ch]->setName(QString("Peak: %2 Hz")
                                  .arg(peakFreq, 0, 'f', 1)); // 1 ondalıklı gösterim
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























// FFTPlotterWindow::FFTPlotterWindow(QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::FFTPlotterWindow) {
//     ui->setupUi(this);
//     ui->tabWidget->setCurrentIndex(0);

//     QList<QColor> palette = {
//         QColor(0xff0000), // 0 - kırmızı
//         QColor(0x00ff00), // 1 - yeşil
//         QColor(0x0000ff), // 2 - mavi
//         QColor(0xffff00), // 3 - sarı
//         QColor(0xff00ff), // 4 - magenta
//         QColor(0x00ffff), // 5 - cyan
//         QColor(0xffaa00),
//         QColor(0x00ffaa),
//         QColor(0xaa00ff),
//         QColor(0xffffff)
//     };

//     for (int i = 0; i < palette.size(); i++)
//         channelColors[i] = palette[i];


//     ui->plot->yAxis->setRange(-yRange, yRange);

//     ui->plot->setBackground(QColor(0x121212));
//     ui->plot->axisRect()->setBackground(QColor(0x1E1E1E));

//     ui->plot->xAxis->setBasePen(QPen(QColor(0xBBBBBB)));
//     ui->plot->yAxis->setBasePen(QPen(QColor(0xBBBBBB)));

//     ui->plot->xAxis->setTickPen(QPen(QColor(0xBBBBBB)));
//     ui->plot->yAxis->setTickPen(QPen(QColor(0xBBBBBB)));

//     ui->plot->xAxis->setTickLabelColor(QColor(0xFFFFFF));
//     ui->plot->yAxis->setTickLabelColor(QColor(0xFFFFFF));

//     fftworkerthread = new QThread();
//     fftworker = new FFTWorkerMulti();

//     connect(FFTPlotterWindow::addNewFFTData, );

//     fftworker->moveToThread(fftworkerthread);
//     fftworkerthread->start();

// }

// FFTPlotterWindow::~FFTPlotterWindow() {
//     delete ui;
// }

// void FFTPlotterWindow::closeEvent(QCloseEvent *event) {

//     emit graphClosed(this);
//     QWidget::closeEvent(event);
// }


// void FFTPlotterWindow::onNewData(int channel, qint64 timestamp, double value) {

//     emit addNewFFTData(channel, timestamp, value);
// }

// void FFTPlotterWindow::addChannel(int channel) {
//     if (graphMap.contains(channel))
//         return;

//     QCPGraph *g = ui->plot->addGraph();
//     // Rengi ata
//     QColor c = channelColors.contains(channel)
//                    ? channelColors[channel]
//                    : QColor::fromHsv((channel * 36) % 360, 255, 255);  // fallback

//     g->setPen(QPen(c, 1));

//     g->setName(QString("Ch %1").arg(channel));

//     graphMap[channel] = g;
// }

// void FFTPlotterWindow::createSignalSelector(int count) {

//     channelCheckboxes.clear();

//     const int cols = 10;

//     for (int ch = 0; ch < count; ++ch)
//     {
//         QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));

//         QFrame *colorBox = new QFrame();
//         colorBox->setFixedSize(14, 14);
//         colorBox->setFrameStyle(QFrame::Box | QFrame::Plain);

//         QColor c = channelColors[ch];
//         colorBox->setStyleSheet(QString("background-color: %1;").arg(c.name()));

//         // Checkbox + renk kutusu için minik yatay layout
//         QWidget *container = new QWidget();
//         QHBoxLayout *h = new QHBoxLayout(container);
//         h->setContentsMargins(0,0,0,0);
//         h->setSpacing(4);

//         h->addWidget(colorBox);
//         h->addWidget(chk);

//         int row = ch / cols;
//         int col = ch % cols;

//         ui->signals_tab_grid_layout->addWidget(container, row, col);

//         connect(chk, &QCheckBox::toggled, this, [=](bool on){
//             if (on) {
//                 addChannel(ch);
//                 selectedChannels.insert(ch);
//                 emit enableFFTChannel(ch);   // Worker’a bildir
//             } else {
//                 removeChannel(ch);
//                 selectedChannels.remove(ch);
//                 emit disableFFTChannel(ch);  // Worker’a bildir
//             }
//         });

//         channelCheckboxes[ch] = chk;
//     }
// }

// void FFTPlotterWindow::removeChannel(int channel) {
//     if (!graphMap.contains(channel))
//         return;

//     ui->plot->removeGraph(graphMap[channel]);
//     graphMap.remove(channel);
// }


// void FFTPlotterWindow::onFftReadyMulti(const QMap<int, QVector<double>> &freqs,
//                                const QMap<int, QVector<double>> &mags)
// {
//     for (int ch : std::as_const(selectedChannels))
//     {
//         if (!graphMap.contains(ch))
//         {
//             QCPGraph *g = ui->plot->addGraph();
//             g->setPen(QPen(channelColors[ch], 2));
//             g->setAntialiased(false);
//             graphMap[ch] = g;
//         }

//         graphMap[ch]->setData(freqs[ch], mags[ch]);
//     }

//     if (freqs.isEmpty())
//         return;

//     auto firstCh = freqs.firstKey();
//     if (freqs[firstCh].isEmpty())
//         return;

//     ui->plot->xAxis->setRange(0, freqs[firstCh].constLast());
//     ui->plot->yAxis->setRange(-120, 0);
//     ui->plot->replot(QCustomPlot::rpQueuedReplot);
// }

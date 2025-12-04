#include "graphwindow.h"
#include "qlabel.h"
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QDate>

GraphWindow::GraphWindow(QWidget *parent)
    : QWidget{parent} {


    startTime = QDateTime::currentMSecsSinceEpoch();

    chart = new QChart();
    axisX = new QValueAxis();
    axisY = new QValueAxis();

    axisX->setRange(0, 5);
    axisY->setRange(-1.2, 1.2);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing, false);

    // =====================================================
    //  COLLAPSIBLE BUTTON
    // =====================================================
    toggleButton = new QToolButton(this);
    toggleButton->setText("Signals");
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);              // Panel başta kapalı
    toggleButton->setArrowType(Qt::ArrowType::RightArrow);
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // =====================================================
    //  SELECTOR PANEL (başta kapalı)
    // =====================================================
    selectorPanel = new QWidget(this);
    selectorLayout = new QGridLayout();   // Panel'e bağlama yok
    selectorPanel->setLayout(selectorLayout);   // <-- ÖNEMLİ
    selectorPanel->setVisible(false);             // ÖNEMLİ

    // =====================================================
    //  COLLAPSE AÇ/KAPA BAĞLANTISI
    // =====================================================
    connect(toggleButton, &QToolButton::toggled, this, [&](bool open) {
        selectorPanel->setVisible(open);
        toggleButton->setArrowType(open ?
                                       Qt::ArrowType::DownArrow :
                                       Qt::ArrowType::RightArrow);
    });


    // =====================================================
    //  AXIS SETTINGS BUTTON
    // =====================================================
    axisButton = new QToolButton(this);
    axisButton->setText("Axis Settings");
    axisButton->setCheckable(true);
    axisButton->setChecked(false);           // başta kapalı
    axisButton->setArrowType(Qt::RightArrow);
    axisButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // =====================================================
    //  AXIS PANEL
    // =====================================================
    axisPanel = new QWidget(this);
    axisLayout = new QGridLayout();
    axisPanel->setLayout(axisLayout);
    axisPanel->setVisible(false);

    // X axis (time range)
    xRangeSpin = new QSpinBox(this);
    xRangeSpin->setRange(1, 600);   // 1s – 10 dakika
    xRangeSpin->setValue(5);        // default = 5 saniye

    // Y axis min/max
    yMinSpin = new QDoubleSpinBox(this);
    yMinSpin->setRange(-9999, 0);
    yMinSpin->setValue(-1.2);

    yMaxSpin = new QDoubleSpinBox(this);
    yMaxSpin->setRange(0, 9999);
    yMaxSpin->setValue(1.2);

    // Panel layout
    axisLayout->addWidget(new QLabel("X Range (s):"), 0, 0);
    axisLayout->addWidget(xRangeSpin, 0, 1);
    axisLayout->addWidget(new QLabel("Y Min:"), 1, 0);
    axisLayout->addWidget(yMinSpin, 1, 1);
    axisLayout->addWidget(new QLabel("Y Max:"), 2, 0);
    axisLayout->addWidget(yMaxSpin, 2, 1);

    // Collapse bağlantısı
    connect(axisButton, &QToolButton::toggled, this, [&](bool open){
        axisPanel->setVisible(open);
        axisButton->setArrowType(open ?
                                     Qt::DownArrow :
                                     Qt::RightArrow);
    });

    // Axis değerleri değiştikçe güncelle
    connect(xRangeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &GraphWindow::applyAxisSettings);
    connect(yMinSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GraphWindow::applyAxisSettings);
    connect(yMaxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GraphWindow::applyAxisSettings);


    // =====================================================
    //  BUTTON BAR (YAN YANA)
    // =====================================================
    QHBoxLayout *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(toggleButton);   // Signals
    buttonRow->addWidget(axisButton);     // Axis
    buttonRow->addStretch();              // Sağ tarafı boş bırakır


    // =====================================================
    //  PANEL ROW (ALT ALTA)
    // =====================================================
    QVBoxLayout *panelRow = new QVBoxLayout();
    panelRow->addWidget(selectorPanel);
    panelRow->addWidget(axisPanel);


    // =====================================================
    //  MAIN LAYOUT
    // =====================================================
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonRow);     // Butonlar yan yana
    mainLayout->addLayout(panelRow);      // Paneller alt alta
    mainLayout->addWidget(view);          // Chart en altta

    setWindowTitle("Grafik Penceresi");
    resize(800, 500);

    // İlk sinyal eklendiğinde kapanıp açılma durumunu önlüyor
    addChannel(0);
    removeChannel(0);

}

void GraphWindow::closeEvent(QCloseEvent *event) {

    emit graphClosed(this);
    QWidget::closeEvent(event);
}

void GraphWindow::addChannel(int channel) {

    if (seriesMap.contains(channel))
        return;

    // --- 1) UI güncellemeleri bloklansın ---
    chart->blockSignals(true);

    QLineSeries *s = new QLineSeries();
    s->setUseOpenGL(true);
    s->setName(QString("Ch %1").arg(channel));

    chart->addSeries(s);
    s->attachAxis(axisX);
    s->attachAxis(axisY);

    seriesMap[channel] = s;

    // --- 2) UI güncellemeleri geri aç ---
    chart->blockSignals(false);

}

void GraphWindow::onNewData(int channel, qint64 timestamp, double value) {

    if ((channel + 1) > max_channel_count) {
        createSignalSelector(channel + 1);
        max_channel_count = (channel + 1);
    }

    if (!seriesMap.contains(channel))
        return;

    double tSec = timestamp / 1000.0;   // sadece saniyeye çevir

    auto *s = seriesMap[channel];
    s->append(tSec, value);

    if (tSec > xRange)
        axisX->setRange(tSec - xRange, tSec);
}


void GraphWindow::createSignalSelector(int count) {

    selectorPanel->setUpdatesEnabled(false);

    while (QLayoutItem *item = selectorLayout->takeAt(0)) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    channelCheckboxes.clear();

    const int cols = 4;

    for (int ch = 0; ch < count; ++ch)
    {
        QCheckBox *chk = new QCheckBox(QString("Ch %1").arg(ch));

        int row = ch / cols;
        int col = ch % cols;

        selectorLayout->addWidget(chk, row, col);

        connect(chk, &QCheckBox::toggled, this, [=](bool on){
            if (on) addChannel(ch);
            else    removeChannel(ch);
        });

        channelCheckboxes[ch] = chk;
    }

    selectorPanel->setUpdatesEnabled(true);

}

void GraphWindow::removeChannel(int channel) {
    if (!seriesMap.contains(channel))
        return;

    auto *series = seriesMap[channel];
    chart->removeSeries(series);
    seriesMap.remove(channel);
    series->deleteLater();
}


void GraphWindow::applyAxisSettings() {
    xRange = static_cast<double>(xRangeSpin->value());       // saniye
    double yMin = yMinSpin->value();
    double yMax = yMaxSpin->value();

    axisY->setRange(yMin, yMax);

    // X ekseni için mevcut timestamp'e göre ayarlanacak,
    // onNewData'da güncelliyorsun → ayar flag’i eklenebilir.
}

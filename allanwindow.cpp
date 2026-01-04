#include "allanwindow.h"
#include "ui_allanwindow.h"
#include <QVBoxLayout>
#include "allancalculator.h"

AllanWindow::AllanWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AllanWindow)
{
    ui->setupUi(this);

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

    ui->plot->setBackground(QColor(53,53,53));
    ui->plot->axisRect()->setBackground(QColor(53,53,53));

    ui->plot->xAxis->setBasePen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setBasePen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickPen(QPen(QColor(0xBBBBBB)));
    ui->plot->yAxis->setTickPen(QPen(QColor(0xBBBBBB)));

    ui->plot->xAxis->setTickLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setTickLabelColor(QColor(0xFFFFFF));

    ui->plot->xAxis->setLabelColor(QColor(0xFFFFFF));
    ui->plot->yAxis->setLabelColor(QColor(0xFFFFFF));

    ui->plot->xAxis->setLabelFont(QFont("Arial", 8));
    ui->plot->yAxis->setLabelFont(QFont("Arial", 8));

    ui->plot->setInteractions(
        QCP::iRangeZoom |      // Mouse wheel ile zoom
        QCP::iRangeDrag        // Mouse drag ile pan
        );
}

AllanWindow::~AllanWindow()
{
    delete ui;
}


void AllanWindow::AllanPlotWindow(const AllanCalculator::Result results)
{
    if (results.channels.isEmpty())
        return;

    ui->plot->setWindowTitle("Allan Deviation");
    ui->plot->resize(900, 650);
    ui->plot->show();

    /* ---------- Axes ---------- */
    ui->plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->plot->yAxis->setScaleType(QCPAxis::stLogarithmic);

    ui->plot->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>::create());
    ui->plot->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog>::create());

    ui->plot->xAxis->setLabel("Tau [s]");
    ui->plot->yAxis->setLabel("Allan Deviation");

    /* ---------- Legend ---------- */
    ui->plot->legend->setVisible(true);
    ui->plot->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));
    ui->plot->legend->setBorderPen(QPen(Qt::black));
    ui->plot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignRight);

    ui->plot->xAxis->setNumberFormat("eb"); // e-notation, beautiful
    ui->plot->yAxis->setNumberFormat("eb");

    ui->plot->xAxis->setNumberPrecision(0);
    ui->plot->yAxis->setNumberPrecision(0);

    ui->plot->xAxis->setSubTicks(true);
    ui->plot->yAxis->setSubTicks(true);

    ui->plot->xAxis->setTickLength(0, 4);
    ui->plot->yAxis->setTickLength(0, 4);


    /* ---------- Channels ---------- */
    for (int ch = 0; ch < results.channels.size(); ++ch)
    {
        const auto& res = results.channels[ch];

        ui->plot->addGraph();

        QPen pen;
        pen.setColor(channelColors[ch % channelColors.size()]);
        pen.setWidth(2);

        ui->plot->graph(ch)->setPen(pen);
        ui->plot->graph(ch)->setLineStyle(QCPGraph::lsLine);
        QCPScatterStyle scatter(QCPScatterStyle::ssCircle, pen.color(), Qt::white, 6);
        ui->plot->graph(ch)->setScatterStyle(scatter);

        ui->plot->graph(ch)->setData(res.tau, res.allanDev);
        QString legendText;

        legendText += QString("Ch%1").arg(ch + 1);

        legendText += QString(
                          "\nARW = %1 (τ = %2 s)")
                          .arg(res.arw, 0, 'g', 4)
                          .arg(res.tauArw, 0, 'g', 3);

        legendText += QString(
                          "\nBias = %1 (τ = %2 s)")
                          .arg(res.bias, 0, 'g', 4)
                          .arg(res.tauBias, 0, 'g', 3);

        ui->plot->graph(ch)->setName(legendText);

        ui->plot->xAxis->setRange(results.channels[0].tau.first(), results.channels[0].tau.last());
        ui->plot->yAxis->rescale(true);
    }

    ui->plot->replot();

}

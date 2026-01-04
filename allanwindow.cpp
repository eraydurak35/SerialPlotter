#include "allanwindow.h"
#include "ui_allanwindow.h"
#include <QVBoxLayout>

AllanWindow::AllanWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AllanWindow)
{
    ui->setupUi(this);
}

AllanWindow::~AllanWindow()
{
    delete ui;
}


void AllanWindow::AllanPlotWindow(const QVector<double>& tau, const QVector<double>& allanDev)
{
    setWindowTitle("Allan Deviation Plot");
    resize(800, 600);

    // Log-log eksen
    ui->plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->plot->yAxis->setScaleType(QCPAxis::stLogarithmic);

    ui->plot->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>::create());
    ui->plot->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog>::create());

    ui->plot->xAxis->setLabel("Tau (s)");
    ui->plot->yAxis->setLabel("Allan Deviation");

    ui->plot->xAxis->setNumberFormat("eb");
    ui->plot->yAxis->setNumberPrecision(0);
    ui->plot->yAxis->setNumberFormat("eb");
    ui->plot->yAxis->setNumberPrecision(0);

    // Grafik
    QCPGraph *graph = ui->plot->addGraph();
    graph->setData(tau, allanDev);
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    // Aralık
    ui->plot->xAxis->setRange(tau.first(), tau.last());
    ui->plot->yAxis->rescale(true);

    ui->plot->replot();
}

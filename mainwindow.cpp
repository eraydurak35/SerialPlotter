#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <plotterwindow.h>
#include <fftplotterwindow.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setWindowTitle(QString("Serial Analizer"));
    scanSerialPorts();

    serialThread = new QThread();
    serialHandler = new SerialHandler();
    serialHandler->moveToThread(serialThread);
    serialThread->start();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::scanSerialPorts() {
    ui->serialPorts->clear();   // önce temizle

    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        ui->serialPorts->addItem(info.portName());
    }

    // Eğer port yoksa kullanıcıya bilgi ver
    if (ui->serialPorts->count() == 0)
        ui->serialPorts->addItem("No COM Ports");
}

void MainWindow::on_scanButton_clicked() {
    scanSerialPorts();
}


void MainWindow::on_connectButton_clicked() {

}

void MainWindow::on_addGraphButton_clicked() {

    PlotterWindow* gw = new PlotterWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    plotList.append(gw);
    gw->setGraphIndex(plotList.size());

    connect(gw, &PlotterWindow::graphClosed, this, &MainWindow::onGraphDestroyed);
    connect(serialHandler, &SerialHandler::newData, gw, &PlotterWindow::onNewData);
    gw->show();

}

void MainWindow::onGraphDestroyed(QObject* obj) {

    PlotterWindow* gw = static_cast<PlotterWindow*>(obj);
    plotList.removeOne(gw);
}

void MainWindow::onFFTGraphDestroyed(QObject* obj) {

    FFTPlotterWindow* gw = static_cast<FFTPlotterWindow*>(obj);
    fftList.removeOne(gw);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Tüm açık GraphWindow’ları kapat
    for (PlotterWindow* gw : std::as_const(plotList)) {
        if (gw && gw->isVisible())
            gw->close();       // pencereyi kapatır
    }

    for (FFTPlotterWindow* fft : std::as_const(fftList)) {
        if (fft && fft->isVisible())
            fft->close();
    }

    event->accept();
}

void MainWindow::on_addFFTGraph_clicked() {

    FFTPlotterWindow* gw = new FFTPlotterWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    fftList.append(gw);
    gw->setGraphIndex(fftList.size());

    connect(gw, &FFTPlotterWindow::graphClosed, this, &MainWindow::onGraphDestroyed);
    connect(serialHandler, &SerialHandler::newData, gw, &FFTPlotterWindow::onNewData);
    gw->show();
}


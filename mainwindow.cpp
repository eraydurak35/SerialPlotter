#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <plotterwindow.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);
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

    // GraphWindow *g = new GraphWindow();
    // g->addChannel(0);
    // g->addChannel(2);
    // connect(serialHandler, &SerialHandler::newData, g, &GraphWindow::onNewData);
    // g->show();
}

void MainWindow::on_addGraphButton_clicked() {


    // PlotterWindow *pw = new PlotterWindow();
    // pw->setAttribute(Qt::WA_DeleteOnClose);  // pencere kapanınca hafızadan silinsin
    // pw->show();


    PlotterWindow* gw = new PlotterWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    plotList.append(gw);

    connect(gw, &PlotterWindow::graphClosed, this, &MainWindow::onGraphDestroyed);
    connect(serialHandler, &SerialHandler::newData, gw, &PlotterWindow::onNewData);
    gw->show();


    // GraphWindow* gw = new GraphWindow(nullptr);
    // gw->setAttribute(Qt::WA_DeleteOnClose);
    // graphList.append(gw);

    // connect(gw, &GraphWindow::graphClosed, this, &MainWindow::onGraphDestroyed);
    // connect(serialHandler, &SerialHandler::newData, gw, &GraphWindow::onNewData);
    // gw->show();
}

void MainWindow::on_pushButton_clicked()
{
    // FFTGraph *fft = new FFTGraph();
    // fft->setAttribute(Qt::WA_DeleteOnClose);
    // fftList.append(fft);

    // connect(fft, &FFTGraph::graphClosed, this, &MainWindow::onFFTGraphDestroyed);
    // connect(serialHandler, &SerialHandler::newData, fft, &FFTGraph::onNewData);
    // fft->show();


    // PlotterWindow *pw = new PlotterWindow();
    // pw->setAttribute(Qt::WA_DeleteOnClose);  // pencere kapanınca hafızadan silinsin
    // pw->show();
}

void MainWindow::onGraphDestroyed(QObject* obj) {

    GraphWindow* gw = static_cast<GraphWindow*>(obj);
    graphList.removeOne(gw);
}

void MainWindow::onFFTGraphDestroyed(QObject* obj) {

    FFTGraph* gw = static_cast<FFTGraph*>(obj);
    fftList.removeOne(gw);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Tüm açık GraphWindow’ları kapat
    for (GraphWindow* gw : graphList) {
        if (gw && gw->isVisible())
            gw->close();       // pencereyi kapatır
    }

    for (FFTGraph* fft : fftList) {
        if (fft && fft->isVisible())
            fft->close();
    }

    event->accept();
}



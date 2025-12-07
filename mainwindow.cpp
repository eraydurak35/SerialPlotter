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

    connect(serialHandler, &SerialHandler::connected, this, &MainWindow::onSerialConnected, Qt::QueuedConnection);
    connect(serialHandler, &SerialHandler::disconnected, this, &MainWindow::onSerialDisconnected, Qt::QueuedConnection);

    connect(this, &MainWindow::requestOpenPort, serialHandler, &SerialHandler::openPort, Qt::QueuedConnection);
    connect(this, &MainWindow::requestClosePort, serialHandler, &SerialHandler::closePort, Qt::QueuedConnection);
    connect(serialHandler, &SerialHandler::statusMessage, this, &MainWindow::showSerialPopup, Qt::QueuedConnection);
}

MainWindow::~MainWindow() {

    serialThread->quit();
    serialThread->wait();

    delete serialHandler;
    delete serialThread;
    delete ui;
}

void MainWindow::showSerialPopup(const QString &msg) {

    // QMessageBox::information(this, "Serial Message", msg);

    QMessageBox *box = new QMessageBox(QMessageBox::Information,
                                       "Serial Message",
                                       msg,
                                       QMessageBox::Ok,
                                       this);

    box->setAttribute(Qt::WA_DeleteOnClose);

    // 1 saniye sonra otomatik kapanır
    QTimer::singleShot(1000, box, &QMessageBox::accept);

    box->show();
}

void MainWindow::onSerialConnected() {

    ui->connectButton->setText("Disconnect");
}

void MainWindow::onSerialDisconnected() {

    ui->connectButton->setText("Connect");
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

    QString port = ui->serialPorts->currentText();
    int baud = ui->baudRates->currentText().toInt();

    if (serialHandler->isSerialOpen()) {
        emit requestClosePort();
    }
    else {
        emit requestOpenPort(port, baud);
    }
}

void MainWindow::on_addGraphButton_clicked() {

    PlotterWindow* gw = new PlotterWindow();
    gw->setAttribute(Qt::WA_DeleteOnClose);
    plotList.append(gw);
    gw->setGraphIndex(plotList.size());

    connect(gw, &PlotterWindow::graphClosed, this, &MainWindow::onGraphDestroyed);
    connect(serialHandler, &SerialHandler::newDataPacket, gw, &PlotterWindow::onNewData);
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
    connect(serialHandler, &SerialHandler::newDataPacket, gw, &FFTPlotterWindow::onNewData);
    gw->show();
}


void MainWindow::on_showTableView_clicked() {

    tableWindow = new DataTableWindow();
    tableWindow->show();

    // SerialHandler sinyalini tabloya bağla
    connect(serialHandler, &SerialHandler::newDataPacket,
            tableWindow, &DataTableWindow::onNewData);
}


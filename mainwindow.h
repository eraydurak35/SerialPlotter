#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plotterwindow.h"
#include "fftplotterwindow.h"
#include "serialhandler.h"
#include <QTimer>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    static constexpr uint8_t MAX_GRAPHS = 16;

    QList<PlotterWindow*> plotList;
    QList<FFTPlotterWindow*> fftList;
    SerialHandler *serialHandler = nullptr;
    QThread *serialThread = nullptr;

    void scanSerialPorts();
    void on_new_serial_data(int channel, qint64 timestamp, double value);
    void onGraphDestroyed(QObject* obj);
    void onFFTGraphDestroyed(QObject* obj);
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_scanButton_clicked();
    void on_connectButton_clicked();
    void on_addGraphButton_clicked();
    void on_addFFTGraph_clicked();
};
#endif // MAINWINDOW_H

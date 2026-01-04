#include "allanvariance.h"
#include "qthread.h"
#include "ui_allanvariance.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QtMath>
#include "allanwindow.h"

AllanVariance::AllanVariance(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AllanVariance)
{
    ui->setupUi(this);
    setWindowTitle(QString("Allan Variance"));

    connect(ui->SelectFileButton, &QPushButton::clicked,
            this, &AllanVariance::onSelectFileButtonClicked);

    connect(ui->ComputeButton, &QPushButton::clicked,
            this, &AllanVariance::onComputeButtonClicked);

    connect(ui->PlotResults, &QPushButton::clicked,
            this, &AllanVariance::onPlotResultsClicked);
}

AllanVariance::~AllanVariance()
{
    delete ui;
}

void AllanVariance::onSelectFileButtonClicked() {

    m_csvPath = QFileDialog::getOpenFileName(
        this,
        "CSV Dosyasi Sec",
        QString(),
        "CSV Files (*.csv)");

    if (!m_csvPath.isEmpty())
        ui->FileNameLabel->setText("File Name: " + m_csvPath);
}


void AllanVariance::onComputeButtonClicked() {

    if (m_csvPath.isEmpty())
    {
        QMessageBox::warning(this, "Hata", "CSV secilmedi");
        return;
    }

    ui->ProgressBar->setValue(0);
    ui->ProgressBar->setVisible(true);

    QThread *thread = new QThread(this);
    AllanCalculator *calc = new AllanCalculator();

    calc->moveToThread(thread);

    connect(thread, &QThread::started, [=]() {
        calc->process(m_csvPath, 5); // ui->channelComboBox->currentIndex() index daha sonra verilecek
    });

    connect(calc, &AllanCalculator::progress,
            ui->ProgressBar, &QSlider::setValue);

    connect(calc, &AllanCalculator::finished,
            this, [=](const AllanCalculator::Result& r) {
                m_result = r;
                QMessageBox::information(this, "Tamam", "Hesaplama bitti");

                qDebug() << "ARW: " << calc->computeARW(m_result.tau, m_result.allanDev);
                double tau = 0;
                qDebug() << "Bias Instability: " << 3600.0 * calc->computeBiasInstability(m_result.tau, m_result.allanDev, tau);
                qDebug() << "Min IDX: " << tau;
                thread->quit();
            });

    connect(calc, &AllanCalculator::error,
            this, [=](const QString& msg) {
                QMessageBox::critical(this, "Hata", msg);
                thread->quit();
            });

    connect(thread, &QThread::finished, calc, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void AllanVariance::onPlotResultsClicked() {

    if (m_result.tau.isEmpty())
    {
        QMessageBox::warning(this, "Hata", "Once hesaplama yapin");
        return;
    }

    AllanWindow *plotWin = new AllanWindow();
    plotWin->AllanPlotWindow(m_result.tau, m_result.allanDev);

    plotWin->setAttribute(Qt::WA_DeleteOnClose); // memory leak yok
    plotWin->show();
}


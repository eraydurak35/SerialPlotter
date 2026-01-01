#include "logger.h"
#include "datapacket.h"
#include "qpushbutton.h"
#include "ui_logger.h"

#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>

Logger::Logger(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Logger)
{
    ui->setupUi(this);

    setWindowTitle(QString("Logger"));
    ui->LogButton->setText("Start Logging");

    connect(ui->LogButton, &QPushButton::clicked, this, &Logger::onLogButtonClicked);

    ui->FileNameLabel->setText("File Name: -");
    ui->LoggedRowLabel->setText("Logged Rows: 0");
    ui->TimeLabel->setText("Time: 00:00:00");
    ui->LogFrequencyLabel->setText("Log Freq: 0 Hz");
}

Logger::~Logger()
{
    stopLogging();
    delete ui;
}

void Logger::onLogButtonClicked()
{
    if (!isLogging)
        startLogging();
    else
        stopLogging();
}

void Logger::startLogging()
{
    QString defaultName =
        QString("log_%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Select log file",
        QDir::homePath() + "/" + defaultName,
        "CSV Files (*.csv)"
        );

    if (filePath.isEmpty())
        return;

    file.setFileName(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file for logging");
        return;
    }

    stream.setDevice(&file);
    stream.setLocale(QLocale::c());   // decimal point '.'

    headerWritten = false;   // <-- ÖNEMLİ
    channelCount = 0;

    loggedRowCount = 0;
    freqCounter = 0;
    lastFreqUpdateTs = 0;

    logTimer.start();

    ui->FileNameLabel->setText("File Name: " + QFileInfo(file.fileName()).fileName());
    ui->LoggedRowLabel->setText("Logged Rows: 0");
    ui->TimeLabel->setText("Time: 00:00:00");
    ui->LogFrequencyLabel->setText("Log Freq: 0 Hz");

    isLogging = true;
    ui->LogButton->setText("Stop Logging");
}


void Logger::stopLogging()
{
    QMutexLocker locker(&mutex);

    if (file.isOpen()) {
        stream.flush();
        file.close();
    }

    isLogging = false;

    ui->LogButton->setText("Start Logging");
    ui->LogFrequencyLabel->setText("Log Freq: 0 Hz");
}

void Logger::logData(DataPacket packet)
{
    if (!isLogging)
        return;

    QMutexLocker locker(&mutex);

    if (!file.isOpen())
        return;

    // --- HEADER AUTO CREATE ---
    if (!headerWritten) {
        channelCount = packet.values.size();

        stream << "timestamp,data_frequency";

        for (int i = 0; i < channelCount; ++i)
            stream << ",ch" << (i + 1);

        stream << "\n";
        headerWritten = true;
    }

    // --- DATA ROW ---
    stream << packet.timestamp;
    stream << "," << QString::number(packet.data_frequency, 'f', 0);

    for (float v : packet.values) {
        stream << "," << QString::number(v, 'f', 3);
    }
    stream << "\n";


    // ---- COUNTERS ----
    loggedRowCount++;

    // ---- UI UPDATES ----
    ui->LoggedRowLabel->setText("Logged Rows: " + QString::number(loggedRowCount));

    // Logging duration
    qint64 elapsedMs = logTimer.elapsed();
    QTime t(0, 0);
    ui->TimeLabel->setText("Time: " + t.addMSecs(elapsedMs).toString("hh:mm:ss"));

    // Log frequency (from packet)
    ui->LogFrequencyLabel->setText("Log Freq: " + QString("%1 Hz").arg(int(packet.data_frequency)));
}

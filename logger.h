#ifndef LOGGER_H
#define LOGGER_H

#include "datapacket.h"
#include "qdir.h"
#include "qelapsedtimer.h"
#include "qmutex.h"
#include <QWidget>

namespace Ui {
class Logger;
}

class Logger : public QWidget
{
    Q_OBJECT

public:
    explicit Logger(QWidget *parent = nullptr);
    ~Logger();

public slots:
    // DSP pipeline veya data source buraya bağlanacak
    void logData(DataPacket packet);

private slots:
    void onLogButtonClicked();

private:
    Ui::Logger *ui;

    void startLogging();
    void stopLogging();

    QFile file;
    QTextStream stream;
    QMutex mutex;

    bool isLogging = false;
    bool headerWritten = false;
    int channelCount = 0;
    quint64 loggedRowCount = 0;
    QElapsedTimer logTimer;

    qint64 lastFreqUpdateTs = 0;
    quint64 freqCounter = 0;
};

#endif // LOGGER_H

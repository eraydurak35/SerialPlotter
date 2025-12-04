#include "serialhandler.h"
#include <QDate>

SerialHandler::SerialHandler(QObject *parent)
    : QObject{parent} {

    // // her sinyale farklı faz verelim
    // for (int i = 0; i < 5; i++)
    //     phase[i] = i * M_PI / 4.0;  // 45° faz farkı

    // startTime = QDateTime::currentMSecsSinceEpoch();

    // connect(&timer, &QTimer::timeout, this, &SerialHandler::generateFakeData);
    // timer.start(10); // 50 Hz


    // Farklı faz başlangıcı
    for (int i = 0; i < 5; i++)
        phase[i] = i * M_PI / 4.0;  // 45°

    // Frekanslar (Hz)
    freqs[0] = 20;
    freqs[1] = 40;
    freqs[2] = 60;
    freqs[3] = 80;
    freqs[4] = 100;

    startTime = QDateTime::currentMSecsSinceEpoch();

    connect(&timer, &QTimer::timeout, this, &SerialHandler::generateFakeData);
    timer.start(1);   // 1 ms → 1000 Hz
}

void SerialHandler::generateFakeData() {
    // qint64 t = QDateTime::currentMSecsSinceEpoch() - startTime;
    // double timeSec = t / 100.0;

    // for (int ch = 0; ch < 5; ch++) {
    //     double value = sin(2.0 * M_PI * 1.0 * timeSec + phase[ch]);

    //     emit newData(ch, t, value);
    // }


    qint64 t = QDateTime::currentMSecsSinceEpoch() - startTime;
    double timeSec = t / 1000.0; // doğru zaman

    for (int ch = 0; ch < 5; ch++)
    {
        double value = sin(2.0 * M_PI * freqs[ch] * timeSec + phase[ch]);

        emit newData(ch, t, value);
    }

}

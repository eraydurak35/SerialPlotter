#include "serialhandler.h"
#include <QDate>

SerialHandler::SerialHandler(QObject *parent)
    : QObject{parent} {

    // Farklı faz başlangıcı
    for (int i = 0; i < 5; i++)
        phase[i] = i * M_PI / 4.0;  // 45°

    // Frekanslar (Hz)
    freqs[0] = 100;
    freqs[1] = 120;
    freqs[2] = 230;
    freqs[3] = 340;
    freqs[4] = 450;

    startTime = QDateTime::currentMSecsSinceEpoch();

    connect(&timer, &QTimer::timeout, this, &SerialHandler::generateFakeData);
    timer.start(1);   // 1 ms → 1000 Hz
}

void SerialHandler::generateFakeData() {

    static qint64 timestamp_ms = 0;
    timestamp_ms++;
    double timeSec = timestamp_ms / 1000.0; // doğru zaman

    for (int ch = 0; ch < 5; ch++)
    {
        double value = sin(2.0 * M_PI * freqs[ch] * timeSec + phase[ch]);
        value += 2.0;

        emit newData(ch, timestamp_ms, value);
    }

}

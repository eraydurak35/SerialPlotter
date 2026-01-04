#include "serialhandler.h"
#include <QDate>

SerialHandler::SerialHandler(QObject *parent)
    : QObject{parent} {

#ifndef DEMO_MODE

    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, &SerialHandler::onReadyRead);

#else
    initDemoMode();
#endif
}

SerialHandler::~SerialHandler() {

    if (serial->isOpen()) {
        serial->close();
    }
}

void SerialHandler::openPort(QString portName, int baudrate)
{
    if (serial->isOpen())
        serial->close();

    serial->setPortName(portName);
    serial->setBaudRate(baudrate);

    if (serial->open(QIODevice::ReadWrite)) {
        emit statusMessage("Connected to " + portName);
        emit connected();
    } else {
        emit statusMessage("Failed to open port!");
    }
}

void SerialHandler::closePort()
{
    if (serial->isOpen()) {
        serial->close();
        emit disconnected();
        emit statusMessage("Disconnected");
    }
}

uint32_t SerialHandler::crc32_update(uint32_t crc, uint8_t data)
{
    crc ^= data;
    for (int i = 0; i < 8; i++)
        crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    return crc;
}

uint32_t SerialHandler::crc32_compute(const uint8_t *data, int length)
{
    uint32_t crc = ~0U; // initial value = 0xFFFFFFFF
    for (int i = 0; i < length; i++)
        crc = crc32_update(crc, data[i]);
    return ~crc; // final XOR
}

void SerialHandler::onReadyRead()
{

    rxLineBuffer.append(serial->readAll());

    while (true) {

        int newlineIndex = rxLineBuffer.indexOf('\n');
        if (newlineIndex < 0)
            return;

        QByteArray line = rxLineBuffer.left(newlineIndex);
        rxLineBuffer.remove(0, newlineIndex + 1);

        line = line.trimmed();
        if (line.isEmpty())
            continue;

        // -----------------------------
        // 1) '*' işaretini bul
        // -----------------------------
        int starIndex = line.lastIndexOf('*');
        if (starIndex < 0) {
            qDebug() << "CSV CRC missing '*' marker";
            continue;
        }

        QByteArray payload = line.left(starIndex);
        QByteArray crcStr  = line.mid(starIndex + 1);

        // -----------------------------
        // 2) CRC32 HEX → sayı
        // -----------------------------
        bool ok = false;
        uint32_t receivedCrc = crcStr.toUInt(&ok, 16);
        if (!ok) {
            qDebug() << "CSV CRC parse error";
            continue;
        }

        // -----------------------------
        // 3) CRC32 Hesapla (asıl düzeltme burada!)
        // -----------------------------
        uint32_t calculatedCrc =
            crc32_compute(
                reinterpret_cast<const uint8_t*>(payload.constData()),
                payload.size()
                );

        if (calculatedCrc != receivedCrc) {
            qDebug() << "CSV CRC mismatch. calc="
                     << QString::number(calculatedCrc, 16)
                     << " recv="
                     << QString::number(receivedCrc, 16);
            continue; // bozuk paket
        }

        // -----------------------------
        // 4) CSV parse et
        // -----------------------------
        QList<QByteArray> parts = payload.split(',');
        if (parts.size() < 2) {
            qDebug() << "CSV parse error: too few fields";
            continue;
        }

        DataPacket packet;

        // Timestamp
        packet.timestamp = parts[0].toUInt(&ok);
        if (!ok) {
            qDebug() << "CSV timestamp parse error";
            continue;
        }

        // Float değerler
        for (int i = 1; i < parts.size(); i++)
        {
            float f = parts[i].toFloat(&ok);
            if (!ok) {
                qDebug() << "CSV float parse error";
                f = 0.0f;
            }
            packet.values.append(f);
        }

        static int i = 0;
        static float data_freq = 0;
        static uint32_t window_start_timestamp = 0;
        static bool is_initialized = false;

        if (!is_initialized) {
            window_start_timestamp = packet.timestamp;
            is_initialized = true;
        }

        if (i++ >= 199) {
            i = 0;
            float time_diff = static_cast<float>(packet.timestamp) - static_cast<float>(window_start_timestamp);
            window_start_timestamp = packet.timestamp;
            if (time_diff > 0) {
                data_freq = 200000.0f / time_diff;
            }
        }

        packet.data_frequency = data_freq;
        emit newDataPacket(packet);
    }
}

#ifndef DEMO_MODE

#else

void SerialHandler::initDemoMode() {
    // Farklı faz başlangıcı
    for (int i = 0; i < 5; i++)
        phase[i] = i * M_PI / 4.0;  // 45°

    // Frekanslar (Hz)
    freqs[0] = 100;
    freqs[1] = 120;
    freqs[2] = 230;
    freqs[3] = 340;
    freqs[4] = 990;

    startTime = QDateTime::currentMSecsSinceEpoch();

    connect(&timer, &QTimer::timeout, this, &SerialHandler::generateFakeData);
    timer.start(1);   // 1 ms → 1000 Hz
}

void SerialHandler::generateFakeData() {

    static qint64 timestamp_ms = 0;
    timestamp_ms = timestamp_ms + 1;
    double timeSec = timestamp_ms / 1000.0; // doğru zaman

    DataPacket packet;
    packet.timestamp = timestamp_ms;
    packet.values.resize(5);

    for (int ch = 0; ch < 5; ch++) {
        double value = sin(2.0 * M_PI * freqs[ch] * timeSec + phase[ch]);
        value += 2.0;

        packet.values[ch] = value;
    }

    packet.data_frequency = 1000.0f;
    emit newDataPacket(packet);
}

#endif

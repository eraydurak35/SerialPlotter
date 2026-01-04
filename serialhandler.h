#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QTimer>
#include "datapacket.h"
#include "qserialport.h"


//#define DEMO_MODE

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialHandler(QObject *parent = nullptr);
    ~SerialHandler();

    bool isSerialOpen() { return serial != nullptr ? serial->isOpen() : false; };

private:

    enum PARSER {
        IDLE,
        HEADER_FOUND,
        TIMESTAMP_FOUND,
        DATACOUNT_FOUND,
        CHECKSUM_COMPARE
    };

    PARSER Parser{IDLE};

    QSerialPort *serial = nullptr;
    QByteArray rxLineBuffer;

    void serialParser(const uint8_t len, const uint8_t *buffer);
    uint32_t crc32_update(uint32_t crc, uint8_t data);
    uint32_t crc32_compute(const uint8_t *data, int length);

#ifndef DEMO_MODE

#else
    void initDemoMode();
    QTimer timer;
    double phase[5];
    qint64 startTime;
    double freqs[5];
#endif
signals:
    void statusMessage(QString msg);
    void connected();
    void disconnected();
    void newDataPacket(const DataPacket &packet);

public slots:
    void openPort(QString portName, int baudrate);
    void closePort();
    void onReadyRead();
private slots:

#ifndef DEMO_MODE

#else
    void generateFakeData();
#endif
};

#endif // SERIALHANDLER_H


/*
    uint32_t timestamp = esp_timer_get_time() / 1000;

    char buffer[128];

    // Veri satırını string olarak oluştur
    int len = snprintf(buffer, sizeof(buffer),
                       "%ld,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f",
                       timestamp,
                       imu.gyro_dps[X],
                       imu.gyro_dps[Y],
                       imu.gyro_dps[Z],
                       imu.accel_ms2[X],
                       imu.accel_ms2[Y],
                       imu.accel_ms2[Z]);
    // CRC32 hesapla
    uint32_t crc = crc32_compute((uint8_t *)buffer, len);

    // CRC32’yi HEX olarak sonuna ekleyip gönder
    printf("%s*%08lX\n", buffer, crc);
 */

#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QTimer>
#include "datapacket.h"

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialHandler(QObject *parent = nullptr);

private:
    QTimer timer;
    double phase[5];
    qint64 startTime;

    double freqs[5];

signals:
    void newDataPacket(const DataPacket &packet);

private slots:
    void generateFakeData();
};

#endif // SERIALHANDLER_H

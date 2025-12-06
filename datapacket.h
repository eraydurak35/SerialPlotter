#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QVector>
#include <QtGlobal>

struct DataPacket
{
    qint64 timestamp;
    QVector<double> values;   // örn: [ch0, ch1, ch2, ch3, ch4]
};

#endif // DATAPACKET_H

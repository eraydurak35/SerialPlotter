#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QVector>
#include <QtGlobal>

struct DataPacket {
    uint32_t timestamp;
    float data_frequency;
    QVector<float> values;
};

#endif // DATAPACKET_H

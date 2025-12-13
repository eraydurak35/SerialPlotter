#ifndef DSPBASE_H
#define DSPBASE_H

#include "datapacket.h"
#include <QWidget>

class DSPBase
{
public:

    virtual ~DSPBase() = default;
    virtual QVector<float> process(const QVector<float> &in, float freq) = 0;

    // 👇 Config UI hook
    virtual void showConfigDialog(QWidget *parent) = 0;
};


#endif // DSPBASE_H

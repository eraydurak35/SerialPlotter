#ifndef DSPBASE_H
#define DSPBASE_H

#include <QWidget>

class DSPBase
{
public:
    virtual ~DSPBase() {}

    virtual QString name() const = 0;

    // Temel DSP işlemi
    virtual QVector<float> process(const QVector<float>& in) = 0;

    // Parametre arayüzü açmak isterse
    virtual void configure(QWidget *parent = nullptr) { /* optional */ }
};


#endif // DSPBASE_H

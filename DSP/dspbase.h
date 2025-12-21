#ifndef DSPBASE_H
#define DSPBASE_H

#include <QWidget>

class DSPBase
{
public:

    virtual ~DSPBase() = default;
    virtual float process(float in, float fs) = 0;
    virtual void showConfigDialog(QWidget *parent) = 0;
    virtual float latencySeconds(float fs) = 0;

    void setBypass(bool b) { bypass = b; }
    bool isBypassed() const { return bypass; }

protected:
    bool bypass = false;
};


#endif // DSPBASE_H

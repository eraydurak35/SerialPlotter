#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include "dspbase.h"

class MedianFilter : public DSPBase
{
public:

    MedianFilter() {};


    QVector<float> process(const QVector<float>& in, float freq) override {

        return in;
    }

    void showConfigDialog(QWidget *parent) override {


    }

    // int windowSize = 5;

    // QString name() const override { return "Median Filter"; }

    // QVector<float> process(const QVector<float> &in) override;

    // void configure(QWidget *parent = nullptr) override;
};

#endif // MEDIANFILTER_H

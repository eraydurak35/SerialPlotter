#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include "dspbase.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qpushbutton.h"
#include "qspinbox.h"

class LowPassFilter : public DSPBase
{
public:

    LowPassFilter() {}

    float process(float in, float fs) override {
        if (bypass)
            return in;

        alpha = computeAlpha(cutoffHz, fs);
        state += alpha * (in - state);
        return state;
    }

    void showConfigDialog(QWidget *parent) override {

        QDialog dlg(parent);
        dlg.setWindowTitle("LowPass Filter");

        QVBoxLayout *l = new QVBoxLayout(&dlg);
        QDoubleSpinBox *cut = new QDoubleSpinBox();
        cut->setRange(0.1, 500);
        cut->setSuffix(" Hz");
        cut->setValue(cutoffHz);

        QPushButton *ok = new QPushButton("OK");

        l->addWidget(cut);
        l->addWidget(ok);

        QObject::connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

        if (dlg.exec() == QDialog::Accepted)
            cutoffHz = cut->value();
    }
private:

    static float computeAlpha(float fc, float fs) {

        if (fs < 1.0) { fs = 1.0; }
        if (fc < 0.1) { fc = 0.1; }
        float wc = 2.0f * M_PI * fc;
        return wc / (wc + fs);
    }


    float cutoffHz = 5.0f;
    float alpha = 0.1f;
    float state = 0.0f;
};

#endif // LOWPASSFILTER_H

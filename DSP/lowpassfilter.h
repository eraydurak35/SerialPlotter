#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include "dspbase.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"

class LowPassFilter : public DSPBase
{
public:

    LowPassFilter() : cutoffHz(5.0f) {}


    QVector<float> process(const QVector<float>& in, float freq) override {
        sampleHz = freq;

        if (state.size() != in.size())
            state = QVector<float>(in.size(), 0.0f);

        for (int i = 0; i < in.size(); ++i)
            state[i] += alpha * (in[i] - state[i]);

        return state;
    }

    void showConfigDialog(QWidget *parent) override {

        QDialog dlg(parent);
        dlg.setWindowTitle("LowPass Filter");

        QVBoxLayout *l = new QVBoxLayout(&dlg);

        QDoubleSpinBox *cutoff = new QDoubleSpinBox();
        cutoff->setRange(0.1, 1000);
        cutoff->setValue(cutoffHz);
        cutoff->setSuffix(" Hz");

        l->addWidget(new QLabel("Cutoff Frequency"));
        l->addWidget(cutoff);

        QPushButton *ok = new QPushButton("OK");
        l->addWidget(ok);

        QObject::connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

        if (dlg.exec() == QDialog::Accepted) {
            cutoffHz = cutoff->value();
            alpha = computeAlpha(cutoffHz, sampleHz);
        }
    }
private:

    inline float computeAlpha(float cutoffHz, float sampleHz) {
        const float wc = 2.0f * M_PI * cutoffHz;
        return wc / (wc + sampleHz);
    }

    QVector<float> state;
    float cutoffHz;
    float alpha = 0.1f;
    float sampleHz = 100.0f;
};

#endif // LOWPASSFILTER_H

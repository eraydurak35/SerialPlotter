#ifndef LOWPASSFILTER2_H
#define LOWPASSFILTER2_H

#include "DSP/dspbase.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"
class LowPassFilter2 : public DSPBase
{
public:
    LowPassFilter2() {
        resetState();
    };

    float process(float in, float fs) {
        if (bypass)
            return in;

        // Katsayıları her çağrıda güncellemek pahalı değil
        computeCoefficients(fs);

        float out =
            b0 * in +
            b1 * x1 +
            b2 * x2 -
            a1 * y1 -
            a2 * y2;

        // State kaydır
        x2 = x1;
        x1 = in;

        y2 = y1;
        y1 = out;

        return out;
    }

    void showConfigDialog(QWidget *parent) {

        QDialog dlg(parent);
        dlg.setWindowTitle("Butterworth 2nd Order LowPass");

        QVBoxLayout *l = new QVBoxLayout(&dlg);

        QLabel *lbl = new QLabel("Cutoff Frequency (Hz)");
        QDoubleSpinBox *spin = new QDoubleSpinBox();
        spin->setRange(0.1, 1000.0);
        spin->setDecimals(2);
        spin->setValue(cutoffHz);

        QPushButton *ok = new QPushButton("OK");

        l->addWidget(lbl);
        l->addWidget(spin);
        l->addWidget(ok);

        QObject::connect(ok, &QPushButton::clicked,
                         &dlg, &QDialog::accept);

        if (dlg.exec() == QDialog::Accepted) {
            cutoffHz = spin->value();
            resetState();
        }
    }

    float latencySeconds([[maybe_unused]]float fs) {
        constexpr float Q = 0.70710678f;
        return Q / (2.0f * M_PI * cutoffHz);
    }


private:

    // Tasarım parametresi
    float cutoffHz = 10.0f;

    // Biquad katsayıları
    float b0 = 0, b1 = 0, b2 = 0;
    float a1 = 0, a2 = 0;

    // State (delay elements)
    float x1 = 0, x2 = 0;
    float y1 = 0, y2 = 0;

    void resetState() {
        x1 = x2 = y1 = y2 = 0.0f;
    }

    void computeCoefficients(float fs) {
        // Butterworth için Q sabit
        const float Q = 0.70710678f;

        float w0 = 2.0f * M_PI * cutoffHz / fs;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);

        float alpha = sinw0 / (2.0f * Q);

        float a0 = 1.0f + alpha;

        b0 = (1.0f - cosw0) / 2.0f / a0;
        b1 = (1.0f - cosw0)       / a0;
        b2 = (1.0f - cosw0) / 2.0f / a0;

        a1 = -2.0f * cosw0 / a0;
        a2 = (1.0f - alpha) / a0;
    }
};

#endif // LOWPASSFILTER2_H

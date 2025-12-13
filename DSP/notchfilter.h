#ifndef NOTCHFILTER_H
#define NOTCHFILTER_H

#include "DSP/dspbase.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qwidget.h"

class NotchFilter : public DSPBase
{
public:
    NotchFilter() {
        resetState();
    }

    float process(float in, float fs) {
        if (bypass)
            return in;

        computeCoefficients(fs);

        float out =
            b0 * in +
            b1 * x1 +
            b2 * x2 -
            a1 * y1 -
            a2 * y2;

        // state kaydır
        x2 = x1;
        x1 = in;

        y2 = y1;
        y1 = out;

        return out;
    }

    void showConfigDialog(QWidget *parent) {

        QDialog dlg(parent);
        dlg.setWindowTitle("Notch Filter");

        QVBoxLayout *l = new QVBoxLayout(&dlg);

        QLabel *lblFc = new QLabel("Center Frequency (Hz)");
        QDoubleSpinBox *fc = new QDoubleSpinBox();
        fc->setRange(0.1, 2000.0);
        fc->setDecimals(2);
        fc->setValue(centerHz);

        QLabel *lblBw = new QLabel("Bandwidth (Hz)");
        QDoubleSpinBox *bw = new QDoubleSpinBox();
        bw->setRange(0.1, 1000.0);
        bw->setDecimals(2);
        bw->setValue(bandwidthHz);

        QPushButton *ok = new QPushButton("OK");

        l->addWidget(lblFc);
        l->addWidget(fc);
        l->addWidget(lblBw);
        l->addWidget(bw);
        l->addWidget(ok);

        QObject::connect(ok, &QPushButton::clicked,
                         &dlg, &QDialog::accept);

        if (dlg.exec() == QDialog::Accepted) {
            centerHz    = fc->value();
            bandwidthHz = bw->value();
            resetState();
        }
    }

private:
    // Kullanıcı parametreleri
    float centerHz   = 50.0f;   // merkez frekans
    float bandwidthHz = 5.0f;   // bant genişliği

    // Biquad katsayıları
    float b0 = 1, b1 = 0, b2 = 1;
    float a1 = 0, a2 = 0;

    // State
    float x1 = 0, x2 = 0;
    float y1 = 0, y2 = 0;

    void resetState() {
        x1 = x2 = y1 = y2 = 0.0f;
    }

    void computeCoefficients(float fs) {
        // RBJ Audio EQ Cookbook
        // BW -> Q dönüşümü
        float Q = centerHz / bandwidthHz;
        if (Q < 0.1f) Q = 0.1f;

        float w0 = 2.0f * M_PI * centerHz / fs;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);

        float alpha = sinw0 / (2.0f * Q);

        float a0 = 1.0f + alpha;

        b0 =  1.0f / a0;
        b1 = -2.0f * cosw0 / a0;
        b2 =  1.0f / a0;

        a1 = -2.0f * cosw0 / a0;
        a2 = (1.0f - alpha) / a0;
    }
};

#endif // NOTCHFILTER_H

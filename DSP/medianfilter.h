#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include "dspbase.h"
#include "qboxlayout.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include <deque>

class MedianFilter : public DSPBase
{
public:

    MedianFilter() {
        enforceOddWindow();
    };

    float process(float in, [[maybe_unused]]float fs) {
        if (bypass)
            return in;

        buffer.push_back(in);

        if ((int)buffer.size() > windowSize)
            buffer.pop_front();

        // pencere dolmadan median alma → en son değer
        if ((int)buffer.size() < windowSize)
            return in;

        // median hesapla
        std::vector<float> tmp(buffer.begin(), buffer.end());
        std::nth_element(
            tmp.begin(),
            tmp.begin() + tmp.size() / 2,
            tmp.end()
            );

        return tmp[tmp.size() / 2];
    }

    void showConfigDialog(QWidget *parent) {
        QDialog dlg(parent);
        dlg.setWindowTitle("Median Filter");

        QVBoxLayout *l = new QVBoxLayout(&dlg);

        QLabel *lbl = new QLabel("Window Size (odd):");
        QSpinBox *spin = new QSpinBox();
        spin->setRange(1, 101);
        spin->setSingleStep(2);
        spin->setValue(windowSize);

        QPushButton *ok = new QPushButton("OK");

        l->addWidget(lbl);
        l->addWidget(spin);
        l->addWidget(ok);

        QObject::connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

        if (dlg.exec() == QDialog::Accepted) {
            windowSize = spin->value();
            enforceOddWindow();
            buffer.clear();  // state reset (çok önemli)
        }
    }

    float latencySeconds(float fs) {
        return 0.5f * (windowSize - 1) / fs;
    }

private:

    int windowSize = 5;
    std::deque<float> buffer;

    void enforceOddWindow()
    {
        if (windowSize % 2 == 0)
            windowSize += 1;
    }

};

#endif // MEDIANFILTER_H

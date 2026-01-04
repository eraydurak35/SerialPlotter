#ifndef ALLANVARIANCE_H
#define ALLANVARIANCE_H

#include <QWidget>
#include "allancalculator.h"

namespace Ui {
class AllanVariance;
}

class AllanVariance : public QWidget
{
    Q_OBJECT

public:
    explicit AllanVariance(QWidget *parent = nullptr);
    ~AllanVariance();

private slots:
    void onSelectFileButtonClicked();

    void onComputeButtonClicked();

    void onPlotResultsClicked();

private:
    Ui::AllanVariance *ui;
    AllanCalculator m_calculator;
    AllanCalculator::Result m_result;
    QString m_csvPath;
};

#endif // ALLANVARIANCE_H

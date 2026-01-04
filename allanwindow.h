#ifndef ALLANWINDOW_H
#define ALLANWINDOW_H

#include <QWidget>
#include "allancalculator.h"

namespace Ui {
class AllanWindow;
}

class AllanWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AllanWindow(QWidget *parent = nullptr);
    ~AllanWindow();

    void AllanPlotWindow(const AllanCalculator::Result results);

private:
    Ui::AllanWindow *ui;

    QMap<int, QColor> channelColors;
};

#endif // ALLANWINDOW_H

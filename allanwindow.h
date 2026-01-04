#ifndef ALLANWINDOW_H
#define ALLANWINDOW_H

#include <QWidget>

namespace Ui {
class AllanWindow;
}

class AllanWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AllanWindow(QWidget *parent = nullptr);
    ~AllanWindow();

    void AllanPlotWindow(const QVector<double>& tau, const QVector<double>& allanDev);

private:
    Ui::AllanWindow *ui;
};

#endif // ALLANWINDOW_H

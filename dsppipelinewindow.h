#ifndef DSPPIPELINEWINDOW_H
#define DSPPIPELINEWINDOW_H

#include <QWidget>

namespace Ui {
class DSPPipeLineWindow;
}

class DSPPipeLineWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DSPPipeLineWindow(QWidget *parent = nullptr);
    ~DSPPipeLineWindow();

private:
    Ui::DSPPipeLineWindow *ui;
};

#endif // DSPPIPELINEWINDOW_H

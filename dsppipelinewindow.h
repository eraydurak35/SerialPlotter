#ifndef DSPPIPELINEWINDOW_H
#define DSPPIPELINEWINDOW_H

#include "qlistwidget.h"
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

private slots:

    void on_addProcessButton_clicked();

private:
    Ui::DSPPipeLineWindow *ui;

    QList<QListWidgetItem*> listWidgetItemList;
};

#endif // DSPPIPELINEWINDOW_H

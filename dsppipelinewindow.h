#ifndef DSPPIPELINEWINDOW_H
#define DSPPIPELINEWINDOW_H

#include "datapacket.h"
#include "qlistwidget.h"
#include <QWidget>
#include "DSP/dspbase.h"

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
    void onPipelineItemDoubleClicked(QListWidgetItem *item);
    void onPipelineItemSingleClicked(QListWidgetItem *item);

public slots:
    void onNewData(DataPacket packet);

signals:
    void newDSPOutput(DataPacket packet);

private:
    Ui::DSPPipeLineWindow *ui;

    QList<QListWidgetItem*> listWidgetItemList;

    std::vector<std::unique_ptr<DSPBase>> pipeline;
};

#endif // DSPPIPELINEWINDOW_H

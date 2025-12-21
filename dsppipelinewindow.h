#ifndef DSPPIPELINEWINDOW_H
#define DSPPIPELINEWINDOW_H

#include "datapacket.h"
#include "qlabel.h"
#include "qlistwidget.h"
#include <QWidget>
#include "DSP/dspbase.h"
#include "qpushbutton.h"
#include "latencyestimator.h"

struct ChannelPipeline {
    int channelIndex;
    QLabel *label;
    QLabel *theoreticalLabel;
    QLabel *measuredLabel;
    QListWidget *listWidget;
    QPushButton *addButton;
    std::vector<std::unique_ptr<DSPBase>> blocks;
    LatencyEstimator latency_estimator;
};

QT_BEGIN_NAMESPACE
namespace Ui { class DSPPipeLineWindow; }
QT_END_NAMESPACE

class DSPPipeLineWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DSPPipeLineWindow(QWidget *parent = nullptr);
    ~DSPPipeLineWindow();

    void createPipelines(int channelCount);

public slots:
    void onNewData(DataPacket packet);

signals:
    void newDSPOutput(DataPacket packet);

private:

    Ui::DSPPipeLineWindow *ui;
    QList<QListWidgetItem*> listWidgetItemList;
    std::vector<std::unique_ptr<DSPBase>> pipeline;
    std::vector<ChannelPipeline> pipelines;
    int max_channel_count = 0;
    QVector<float> channelTotalLatencySec;


    void rebuildPipelineItemIndices();
    void setupPipelineUI(int pipeIndex);
    void showAddProcessMenu(int pipeIndex);
    void rebuildPipelineIndices(ChannelPipeline &pipe);
};

#endif // DSPPIPELINEWINDOW_H

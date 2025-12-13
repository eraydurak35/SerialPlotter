#include "dsppipelinewindow.h"
#include "DSP/notchfilter.h"
#include "datapacket.h"
#include "DSP/lowpassfilter.h"
#include "DSP/medianfilter.h"
#include "DSP/lowpassfilter2.h"
#include "qmenu.h"
#include "qpushbutton.h"
#include "ui_dsppipelinewindow.h"

DSPPipeLineWindow::DSPPipeLineWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DSPPipeLineWindow) {
    ui->setupUi(this);

    setWindowTitle(QString("DSP Pipeline Editor"));

}

DSPPipeLineWindow::~DSPPipeLineWindow() {
    delete ui;
}

void DSPPipeLineWindow::createPipelines(int channelCount)
{
    pipelines.clear();

    const int cols = 10;

    for (int ch = 0; ch < channelCount; ++ch) {

        ChannelPipeline pipe;
        pipe.channelIndex = ch;

        QWidget *container = new QWidget();
        QVBoxLayout *v = new QVBoxLayout(container);

        pipe.label = new QLabel(QString("Ch %1").arg(ch));
        pipe.label->setAlignment(Qt::AlignCenter);
        pipe.label->setStyleSheet("font-weight: bold;");
        v->addWidget(pipe.label);

        pipe.listWidget = new QListWidget();
        pipe.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        v->addWidget(pipe.listWidget);

        pipe.addButton = new QPushButton("Add Process");
        v->addWidget(pipe.addButton);

        int row = ch / cols;
        int col = ch % cols;
        ui->gridLayout_2->addWidget(container, row, col);

        pipelines.push_back(std::move(pipe));

        const int idx = static_cast<int>(pipelines.size()) - 1;

        setupPipelineUI(idx);
    }
}


void DSPPipeLineWindow::setupPipelineUI(int pipeIndex)
{
    // Güvenlik
    if (pipeIndex < 0 || pipeIndex >= (int)pipelines.size())
        return;

    ChannelPipeline *p = &pipelines[pipeIndex];

    // Add Process
    connect(p->addButton, &QPushButton::clicked, this, [this, pipeIndex]() {
        showAddProcessMenu(pipeIndex);
    });

    // Double click → Configure
    connect(p->listWidget, &QListWidget::itemDoubleClicked,
            this, [this, pipeIndex](QListWidgetItem *item) {

                auto &pipe = pipelines[pipeIndex];

                int idx = item->data(Qt::UserRole).toInt();
                if (idx >= 0 && idx < (int)pipe.blocks.size())
                    pipe.blocks[idx]->showConfigDialog(this);
            });

    // Right click → Pass / Remove
    connect(p->listWidget, &QListWidget::customContextMenuRequested,
            this, [this, pipeIndex](const QPoint &pos) {

                auto &pipe = pipelines[pipeIndex];

                QListWidgetItem *item = pipe.listWidget->itemAt(pos);
                if (!item) return;

                int idx = item->data(Qt::UserRole).toInt();
                if (idx < 0 || idx >= (int)pipe.blocks.size()) return;

                QMenu menu;
                QAction *bypass = menu.addAction(pipe.blocks[idx]->isBypassed() ? "Enable" : "ByPass");
                QAction *remove = menu.addAction("Remove");

                QAction *sel = menu.exec(pipe.listWidget->viewport()->mapToGlobal(pos));
                if (!sel) return;

                if (sel == bypass) {
                    bool b = !pipe.blocks[idx]->isBypassed();
                    pipe.blocks[idx]->setBypass(b);

                    QFont f = item->font();
                    f.setItalic(b);
                    item->setFont(f);
                }
                else if (sel == remove) {
                    delete pipe.listWidget->takeItem(idx);
                    pipe.blocks.erase(pipe.blocks.begin() + idx);
                    rebuildPipelineIndices(pipe);
                }
            });
}

void DSPPipeLineWindow::showAddProcessMenu(int pipeIndex) {

    if (pipeIndex < 0 || pipeIndex >= (int)pipelines.size())
        return;

    auto &pipe = pipelines[pipeIndex];

    QMenu menu;
    QAction *lowpass = menu.addAction("LowPass (1st Order)");
    QAction *lowpass2 = menu.addAction("LowPass (2nd Order)");
    QAction *notch = menu.addAction("Notch");
    QAction *median  = menu.addAction("Median");

    QAction *sel = menu.exec(QCursor::pos());
    if (!sel) return;

    QListWidgetItem *item = new QListWidgetItem();

    if (sel == lowpass) {
        pipe.blocks.push_back(std::make_unique<LowPassFilter>());
        item->setText("LowPass (1st Order)");
    }
    else if (sel == lowpass2) {
        pipe.blocks.push_back(std::make_unique<LowPassFilter2>());
        item->setText("LowPass (2nd Order)");
    }
    else if (sel == notch) {
        pipe.blocks.push_back(std::make_unique<NotchFilter>());
        item->setText("Notch");
    }
    else if (sel == median) {
        pipe.blocks.push_back(std::make_unique<MedianFilter>());
        item->setText("Median");
    }

    item->setData(Qt::UserRole, (int)pipe.blocks.size() - 1);
    pipe.listWidget->addItem(item);
}


void DSPPipeLineWindow::rebuildPipelineIndices(ChannelPipeline &pipe) {
    for (int i = 0; i < pipe.listWidget->count(); ++i)
        pipe.listWidget->item(i)->setData(Qt::UserRole, i);
}


void DSPPipeLineWindow::onNewData(DataPacket packet) {

    if ((packet.values.size()) > max_channel_count) {
        createPipelines(packet.values.size());
        max_channel_count = packet.values.size();
    }

    QVector<float> out = packet.values;

    int n = qMin((int)pipelines.size(), packet.values.size());

    for (int ch = 0; ch < n; ++ch) {
        float x = packet.values[ch];

        for (auto &blk : pipelines[ch].blocks)
            x = blk->process(x, packet.data_frequency);

        out[ch] = x;
    }

    packet.values = out;
    emit newDSPOutput(packet);
}


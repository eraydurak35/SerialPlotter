#include "dsppipelinewindow.h"
#include "datapacket.h"
#include "DSP/lowpassfilter.h"
#include "DSP/medianfilter.h"
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

void DSPPipeLineWindow::on_addProcessButton_clicked()
{

    QMenu menu(this);

    QStringList items = {"Median", "LowPass", "B", "C", "D"};

    for (const QString &s : items) {
        QAction *act = menu.addAction(s);
        connect(act, &QAction::triggered, this, [s, this](){

            QListWidgetItem *item = new QListWidgetItem();


            if (s == "Median") {
                item->setText("Median");
                pipeline.push_back(std::make_unique<MedianFilter>());
            }
            else if (s == "LowPass") {
                item->setText("LowPass");
                pipeline.push_back(std::make_unique<LowPassFilter>());
            }

            item->setData(Qt::UserRole, pipeline.size() - 1);

            connect(ui->PipelineList,
                    &QListWidget::itemDoubleClicked,
                    this,
                    &DSPPipeLineWindow::onPipelineItemDoubleClicked);

            connect(ui->PipelineList,
                    &QListWidget::itemClicked,
                    this,
                    &DSPPipeLineWindow::onPipelineItemSingleClicked);

            listWidgetItemList.append(item);
            ui->PipelineList->addItem(item);

        });
    }

    // Menüyü imleç pozisyonunda aç
    menu.exec(QCursor::pos());
}

void DSPPipeLineWindow::onNewData(DataPacket packet) {

    QVector<float> data = packet.values;

    for (auto& block : pipeline) {

        data = block->process(data, packet.data_frequency);
    }

    packet.values = data;

    emit newDSPOutput(packet);
}


void DSPPipeLineWindow::onPipelineItemDoubleClicked(QListWidgetItem *item)
{

    QMenu menu(this);

    QStringList items = {"PassTrue", "Remove"};

    for (const QString &s : items) {
        QAction *act = menu.addAction(s);
        connect(act, &QAction::triggered, this, [s](){


            if (s == "PassTrue") {

            }
            else if (s == "Remove") {

            }

        });
    }

    // Menüyü imleç pozisyonunda aç
    menu.exec(QCursor::pos());
}

void DSPPipeLineWindow::onPipelineItemSingleClicked(QListWidgetItem *item) {


    int index = item->data(Qt::UserRole).toInt();

    if (index < 0 || index >= pipeline.size())
        return;

    pipeline[index]->showConfigDialog(this);

}

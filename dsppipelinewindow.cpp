#include "dsppipelinewindow.h"
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

// void DSPPipeLineWindow::on_PipelineListClicked(QListWidgetItem *item) {



// }


void DSPPipeLineWindow::on_addProcessButton_clicked()
{

    QMenu menu(this);

    QStringList items = {"Median Filter", "A", "B", "C", "D"};

    for (const QString &s : items) {
        QAction *act = menu.addAction(s);
        connect(act, &QAction::triggered, this, [s, this](){

            QListWidgetItem *item = new QListWidgetItem();
            item->setText("Median Filter");
            listWidgetItemList.append(item);
            ui->PipelineList->addItem(item);

            qDebug() << "Seçilen:" << s;
        });
    }

    // Menüyü imleç pozisyonunda aç
    menu.exec(QCursor::pos());
}


#include "datatablewindow.h"
#include "ui_datatablewindow.h"

DataTableWindow::DataTableWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTableWindow) {
    ui->setupUi(this);

}

DataTableWindow::~DataTableWindow() {
    delete ui;
}

void DataTableWindow::onNewData(DataPacket packet) {

    static bool is_table_created = false;

    if (!is_table_created) {
        is_table_created = true;

        model = new QStandardItemModel(this);

        model->setColumnCount(packet.values.size() + 1);
        model->setHeaderData(0, Qt::Horizontal, "Timestamp (ms)");
        for (int i = 0; i < packet.values.size(); i++) {
            model->setHeaderData(i + 1, Qt::Horizontal, QString("Ch %1").arg(i));
        }

        ui->tableView->setModel(model);

        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->verticalHeader()->setVisible(false);
        ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Otomatik scroll aşağı
        ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    }

    QList<QStandardItem*> row;

    row << new QStandardItem(QString::number(packet.timestamp));
    for (int i = 0; i < packet.values.size(); i++) {
        row << new QStandardItem(QString::number(packet.values[i], 'f', 6));
    }

    model->appendRow(row);

    // En alta scroll
    ui->tableView->scrollToBottom();
}

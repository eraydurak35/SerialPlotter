#ifndef DATATABLEWINDOW_H
#define DATATABLEWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include "datapacket.h"

namespace Ui {
class DataTableWindow;
}

class DataTableWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DataTableWindow(QWidget *parent = nullptr);
    ~DataTableWindow();

public slots:
    void onNewData(DataPacket packet);

private:
    Ui::DataTableWindow *ui;
    QStandardItemModel *model;

    int max_channel_count = 0;
};

#endif // DATATABLEWINDOW_H

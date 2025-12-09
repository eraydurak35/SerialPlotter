#include "dsppipelinewindow.h"
#include "ui_dsppipelinewindow.h"

DSPPipeLineWindow::DSPPipeLineWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DSPPipeLineWindow)
{
    ui->setupUi(this);
}

DSPPipeLineWindow::~DSPPipeLineWindow()
{
    delete ui;
}

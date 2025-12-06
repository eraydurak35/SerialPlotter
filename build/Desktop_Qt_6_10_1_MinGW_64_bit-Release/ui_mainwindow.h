/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *scanButton;
    QComboBox *serialPorts;
    QComboBox *baudRates;
    QPushButton *connectButton;
    QPushButton *addGraphButton;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(254, 294);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(70, 10, 111, 168));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(label);

        scanButton = new QPushButton(verticalLayoutWidget);
        scanButton->setObjectName("scanButton");

        verticalLayout->addWidget(scanButton);

        serialPorts = new QComboBox(verticalLayoutWidget);
        serialPorts->setObjectName("serialPorts");

        verticalLayout->addWidget(serialPorts);

        baudRates = new QComboBox(verticalLayoutWidget);
        baudRates->addItem(QString());
        baudRates->addItem(QString());
        baudRates->setObjectName("baudRates");

        verticalLayout->addWidget(baudRates);

        connectButton = new QPushButton(verticalLayoutWidget);
        connectButton->setObjectName("connectButton");

        verticalLayout->addWidget(connectButton);

        addGraphButton = new QPushButton(verticalLayoutWidget);
        addGraphButton->setObjectName("addGraphButton");

        verticalLayout->addWidget(addGraphButton);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(80, 210, 80, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 254, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Port Selection", nullptr));
        scanButton->setText(QCoreApplication::translate("MainWindow", "Scan", nullptr));
        baudRates->setItemText(0, QCoreApplication::translate("MainWindow", "115200", nullptr));
        baudRates->setItemText(1, QCoreApplication::translate("MainWindow", "921600", nullptr));

        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        addGraphButton->setText(QCoreApplication::translate("MainWindow", "Add Graph", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "FFT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

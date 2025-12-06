/********************************************************************************
** Form generated from reading UI file 'plotterwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTTERWINDOW_H
#define UI_PLOTTERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "qcustomplot/qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_PlotterWindow
{
public:
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *signals_tab;
    QHBoxLayout *horizontalLayout;
    QGridLayout *tab1_gridlayout;
    QWidget *settings;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QDoubleSpinBox *yoffsetspinbox;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QDoubleSpinBox *yrangespinbox;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QDoubleSpinBox *xrrangespinbox;
    QGridLayout *gridLayout;
    QCustomPlot *plot;

    void setupUi(QWidget *PlotterWindow)
    {
        if (PlotterWindow->objectName().isEmpty())
            PlotterWindow->setObjectName("PlotterWindow");
        PlotterWindow->resize(992, 679);
        gridLayout_2 = new QGridLayout(PlotterWindow);
        gridLayout_2->setObjectName("gridLayout_2");
        tabWidget = new QTabWidget(PlotterWindow);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        signals_tab = new QWidget();
        signals_tab->setObjectName("signals_tab");
        horizontalLayout = new QHBoxLayout(signals_tab);
        horizontalLayout->setObjectName("horizontalLayout");
        tab1_gridlayout = new QGridLayout();
        tab1_gridlayout->setObjectName("tab1_gridlayout");

        horizontalLayout->addLayout(tab1_gridlayout);

        tabWidget->addTab(signals_tab, QString());
        settings = new QWidget();
        settings->setObjectName("settings");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(settings->sizePolicy().hasHeightForWidth());
        settings->setSizePolicy(sizePolicy1);
        horizontalLayout_3 = new QHBoxLayout(settings);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_3 = new QLabel(settings);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        yoffsetspinbox = new QDoubleSpinBox(settings);
        yoffsetspinbox->setObjectName("yoffsetspinbox");
        yoffsetspinbox->setMinimum(-99999999.000000000000000);
        yoffsetspinbox->setMaximum(99999999.000000000000000);
        yoffsetspinbox->setSingleStep(0.500000000000000);

        horizontalLayout_2->addWidget(yoffsetspinbox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label = new QLabel(settings);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        yrangespinbox = new QDoubleSpinBox(settings);
        yrangespinbox->setObjectName("yrangespinbox");
        yrangespinbox->setMinimum(1.000000000000000);
        yrangespinbox->setMaximum(9999999.000000000000000);
        yrangespinbox->setSingleStep(0.500000000000000);

        horizontalLayout_2->addWidget(yrangespinbox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        label_2 = new QLabel(settings);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        xrrangespinbox = new QDoubleSpinBox(settings);
        xrrangespinbox->setObjectName("xrrangespinbox");
        xrrangespinbox->setDecimals(2);
        xrrangespinbox->setMinimum(1.000000000000000);
        xrrangespinbox->setMaximum(120.000000000000000);

        horizontalLayout_2->addWidget(xrrangespinbox);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        tabWidget->addTab(settings, QString());

        gridLayout_2->addWidget(tabWidget, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        plot = new QCustomPlot(PlotterWindow);
        plot->setObjectName("plot");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(plot->sizePolicy().hasHeightForWidth());
        plot->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(plot, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);


        retranslateUi(PlotterWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(PlotterWindow);
    } // setupUi

    void retranslateUi(QWidget *PlotterWindow)
    {
        PlotterWindow->setWindowTitle(QCoreApplication::translate("PlotterWindow", "Form", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(signals_tab), QCoreApplication::translate("PlotterWindow", "Signals", nullptr));
        label_3->setText(QCoreApplication::translate("PlotterWindow", "Y Offset -->", nullptr));
        label->setText(QCoreApplication::translate("PlotterWindow", "Y Range -->", nullptr));
        label_2->setText(QCoreApplication::translate("PlotterWindow", "X Range -->", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(settings), QCoreApplication::translate("PlotterWindow", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotterWindow: public Ui_PlotterWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTTERWINDOW_H

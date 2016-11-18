/********************************************************************************
** Form generated from reading UI file 'guitest.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUITEST_H
#define UI_GUITEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "spectrumwidget.hpp"
#include "vumeterwidget.hpp"

QT_BEGIN_NAMESPACE

class Ui_guitest
{
public:
    QWidget *centralWidget;
    VUMeterWidget *vumeter;
    QSlider *ctlVol;
    spectrumWidget *spectrum;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *guitest)
    {
        if (guitest->objectName().isEmpty())
            guitest->setObjectName(QStringLiteral("guitest"));
        guitest->resize(400, 300);
        centralWidget = new QWidget(guitest);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        vumeter = new VUMeterWidget(centralWidget);
        vumeter->setObjectName(QStringLiteral("vumeter"));
        vumeter->setGeometry(QRect(2, 0, 397, 23));
        ctlVol = new QSlider(centralWidget);
        ctlVol->setObjectName(QStringLiteral("ctlVol"));
        ctlVol->setGeometry(QRect(4, 24, 393, 23));
        ctlVol->setMaximum(100);
        ctlVol->setValue(80);
        ctlVol->setOrientation(Qt::Horizontal);
        ctlVol->setTickPosition(QSlider::TicksBelow);
        ctlVol->setTickInterval(5);
        spectrum = new spectrumWidget(centralWidget);
        spectrum->setObjectName(QStringLiteral("spectrum"));
        spectrum->setGeometry(QRect(4, 52, 393, 189));
        spectrum->setFrameShape(QFrame::StyledPanel);
        spectrum->setFrameShadow(QFrame::Raised);
        guitest->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(guitest);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        guitest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(guitest);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        guitest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(guitest);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        guitest->setStatusBar(statusBar);

        retranslateUi(guitest);

        QMetaObject::connectSlotsByName(guitest);
    } // setupUi

    void retranslateUi(QMainWindow *guitest)
    {
        guitest->setWindowTitle(QApplication::translate("guitest", "guitest", 0));
    } // retranslateUi

};

namespace Ui {
    class guitest: public Ui_guitest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUITEST_H

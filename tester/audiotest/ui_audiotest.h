/********************************************************************************
** Form generated from reading UI file 'audiotest.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOTEST_H
#define UI_AUDIOTEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDial>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "vumeterwidget.hpp"

QT_BEGIN_NAMESPACE

class Ui_audiotest
{
public:
    QWidget *centralWidget;
    QDial *ctlVol;
    VUMeterWidget *vumeter;
    QLabel *ledSND;
    QLabel *lblSnd;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *audiotest)
    {
        if (audiotest->objectName().isEmpty())
            audiotest->setObjectName(QStringLiteral("audiotest"));
        audiotest->resize(400, 300);
        centralWidget = new QWidget(audiotest);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ctlVol = new QDial(centralWidget);
        ctlVol->setObjectName(QStringLiteral("ctlVol"));
        ctlVol->setGeometry(QRect(6, 38, 50, 64));
        ctlVol->setMaximum(100);
        ctlVol->setValue(75);
        ctlVol->setNotchTarget(5);
        ctlVol->setNotchesVisible(true);
        vumeter = new VUMeterWidget(centralWidget);
        vumeter->setObjectName(QStringLiteral("vumeter"));
        vumeter->setGeometry(QRect(2, 2, 393, 25));
        ledSND = new QLabel(centralWidget);
        ledSND->setObjectName(QStringLiteral("ledSND"));
        ledSND->setGeometry(QRect(8, 100, 20, 13));
        ledSND->setMidLineWidth(-6);
        ledSND->setPixmap(QPixmap(QString::fromUtf8(":/image/greenled.png")));
        ledSND->setScaledContents(true);
        ledSND->setAlignment(Qt::AlignCenter);
        lblSnd = new QLabel(centralWidget);
        lblSnd->setObjectName(QStringLiteral("lblSnd"));
        lblSnd->setGeometry(QRect(6, 120, 47, 13));
        audiotest->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(audiotest);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        audiotest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(audiotest);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        audiotest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(audiotest);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        audiotest->setStatusBar(statusBar);

        retranslateUi(audiotest);

        QMetaObject::connectSlotsByName(audiotest);
    } // setupUi

    void retranslateUi(QMainWindow *audiotest)
    {
        audiotest->setWindowTitle(QApplication::translate("audiotest", "audiotest", 0));
        ledSND->setText(QString());
        lblSnd->setText(QApplication::translate("audiotest", "SND", 0));
    } // retranslateUi

};

namespace Ui {
    class audiotest: public Ui_audiotest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOTEST_H

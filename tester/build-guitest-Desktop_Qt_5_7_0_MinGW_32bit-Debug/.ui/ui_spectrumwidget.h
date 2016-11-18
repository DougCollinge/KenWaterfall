/********************************************************************************
** Form generated from reading UI file 'spectrumwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPECTRUMWIDGET_H
#define UI_SPECTRUMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include "fftdisplay.hpp"
#include "markerwidget.hpp"

QT_BEGIN_NAMESPACE

class Ui_spectrumWidget
{
public:
    QVBoxLayout *verticalLayout;
    markerWidget *markerLabelSpectrum;
    fftDisplay *spectrWidget;
    markerWidget *markerLabelWF;
    fftDisplay *waterfallWidget;
    QGridLayout *gridLayout;
    QLabel *maxDbLabel;
    QLabel *rangeLabel;
    QLabel *label_3;
    QSpinBox *maxDbSpinbox;
    QSpinBox *rangeSpinbox;
    QDoubleSpinBox *avgDoubleSpinBox;

    void setupUi(QFrame *spectrumWidget)
    {
        if (spectrumWidget->objectName().isEmpty())
            spectrumWidget->setObjectName(QStringLiteral("spectrumWidget"));
        spectrumWidget->resize(515, 250);
        spectrumWidget->setMinimumSize(QSize(0, 0));
        spectrumWidget->setMaximumSize(QSize(1620, 1240));
        QFont font;
        font.setFamily(QStringLiteral("Ubuntu Mono"));
        font.setPointSize(9);
        font.setBold(false);
        font.setWeight(50);
        spectrumWidget->setFont(font);
        spectrumWidget->setFrameShape(QFrame::Panel);
        spectrumWidget->setFrameShadow(QFrame::Sunken);
        spectrumWidget->setLineWidth(2);
        verticalLayout = new QVBoxLayout(spectrumWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        markerLabelSpectrum = new markerWidget(spectrumWidget);
        markerLabelSpectrum->setObjectName(QStringLiteral("markerLabelSpectrum"));
        markerLabelSpectrum->setMaximumSize(QSize(16777215, 8));

        verticalLayout->addWidget(markerLabelSpectrum);

        spectrWidget = new fftDisplay(spectrumWidget);
        spectrWidget->setObjectName(QStringLiteral("spectrWidget"));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 0, 127, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(0, 0, 191, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(0, 0, 159, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(0, 0, 63, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(0, 0, 84, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        QBrush brush7(QColor(255, 255, 220, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush6);
        spectrWidget->setPalette(palette);
        spectrWidget->setAutoFillBackground(true);
        spectrWidget->setProperty("lineWidth", QVariant(1));

        verticalLayout->addWidget(spectrWidget);

        markerLabelWF = new markerWidget(spectrumWidget);
        markerLabelWF->setObjectName(QStringLiteral("markerLabelWF"));
        markerLabelWF->setMaximumSize(QSize(16777215, 8));

        verticalLayout->addWidget(markerLabelWF);

        waterfallWidget = new fftDisplay(spectrumWidget);
        waterfallWidget->setObjectName(QStringLiteral("waterfallWidget"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush2);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush4);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        palette1.setBrush(QPalette::Active, QPalette::BrightText, brush);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush6);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush7);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush7);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush6);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush7);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush6);
        waterfallWidget->setPalette(palette1);
        waterfallWidget->setAutoFillBackground(true);
        waterfallWidget->setProperty("lineWidth", QVariant(1));

        verticalLayout->addWidget(waterfallWidget);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        maxDbLabel = new QLabel(spectrumWidget);
        maxDbLabel->setObjectName(QStringLiteral("maxDbLabel"));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        maxDbLabel->setFont(font1);
        maxDbLabel->setAlignment(Qt::AlignCenter);
        maxDbLabel->setWordWrap(false);

        gridLayout->addWidget(maxDbLabel, 0, 0, 1, 1);

        rangeLabel = new QLabel(spectrumWidget);
        rangeLabel->setObjectName(QStringLiteral("rangeLabel"));
        QFont font2;
        font2.setPointSize(10);
        rangeLabel->setFont(font2);
        rangeLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(rangeLabel, 0, 1, 1, 1);

        label_3 = new QLabel(spectrumWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        maxDbSpinbox = new QSpinBox(spectrumWidget);
        maxDbSpinbox->setObjectName(QStringLiteral("maxDbSpinbox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(maxDbSpinbox->sizePolicy().hasHeightForWidth());
        maxDbSpinbox->setSizePolicy(sizePolicy);
        maxDbSpinbox->setMaximumSize(QSize(16777215, 30));
        maxDbSpinbox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        maxDbSpinbox->setMinimum(-40);
        maxDbSpinbox->setMaximum(10);
        maxDbSpinbox->setValue(0);

        gridLayout->addWidget(maxDbSpinbox, 1, 0, 1, 1);

        rangeSpinbox = new QSpinBox(spectrumWidget);
        rangeSpinbox->setObjectName(QStringLiteral("rangeSpinbox"));
        sizePolicy.setHeightForWidth(rangeSpinbox->sizePolicy().hasHeightForWidth());
        rangeSpinbox->setSizePolicy(sizePolicy);
        rangeSpinbox->setMaximumSize(QSize(16777215, 30));
        rangeSpinbox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        rangeSpinbox->setMinimum(10);
        rangeSpinbox->setMaximum(50);
        rangeSpinbox->setValue(10);

        gridLayout->addWidget(rangeSpinbox, 1, 1, 1, 1);

        avgDoubleSpinBox = new QDoubleSpinBox(spectrumWidget);
        avgDoubleSpinBox->setObjectName(QStringLiteral("avgDoubleSpinBox"));
        avgDoubleSpinBox->setMaximumSize(QSize(16777215, 30));
        avgDoubleSpinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        avgDoubleSpinBox->setMinimum(0.01);
        avgDoubleSpinBox->setMaximum(1);
        avgDoubleSpinBox->setSingleStep(0.01);

        gridLayout->addWidget(avgDoubleSpinBox, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalLayout->setStretch(1, 1);
        verticalLayout->setStretch(3, 3);

        retranslateUi(spectrumWidget);

        QMetaObject::connectSlotsByName(spectrumWidget);
    } // setupUi

    void retranslateUi(QFrame *spectrumWidget)
    {
        spectrumWidget->setWindowTitle(QApplication::translate("spectrumWidget", "Frame", 0));
        markerLabelSpectrum->setText(QString());
        spectrWidget->setProperty("text", QVariant(QString()));
        markerLabelWF->setText(QString());
        waterfallWidget->setProperty("text", QVariant(QString()));
        maxDbLabel->setText(QApplication::translate("spectrumWidget", "Max dB", 0));
        rangeLabel->setText(QApplication::translate("spectrumWidget", "Range", 0));
        label_3->setText(QApplication::translate("spectrumWidget", "Avg", 0));
    } // retranslateUi

};

namespace Ui {
    class spectrumWidget: public Ui_spectrumWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPECTRUMWIDGET_H

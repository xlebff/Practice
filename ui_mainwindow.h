/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *pbclean;
    QPushButton *pbtime;
    QComboBox *comboBox;
    QCustomPlot *widget;
    QPushButton *pbclose;
    QProgressBar *progressBar;
    QPushButton *pbremovefile;
    QPushButton *pbwritefile;
    QPushButton *pbspectrum;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(768, 465);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pbclean = new QPushButton(centralWidget);
        pbclean->setObjectName(QString::fromUtf8("pbclean"));
        pbclean->setCheckable(false);

        gridLayout->addWidget(pbclean, 2, 2, 1, 1);

        pbtime = new QPushButton(centralWidget);
        pbtime->setObjectName(QString::fromUtf8("pbtime"));

        gridLayout->addWidget(pbtime, 2, 0, 1, 1);

        comboBox = new QComboBox(centralWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setMinimumSize(QSize(0, 26));

        gridLayout->addWidget(comboBox, 2, 5, 1, 1);

        widget = new QCustomPlot(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 0, 0, 1, 6);

        pbclose = new QPushButton(centralWidget);
        pbclose->setObjectName(QString::fromUtf8("pbclose"));

        gridLayout->addWidget(pbclose, 3, 5, 1, 1);

        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);
        progressBar->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(progressBar, 3, 0, 1, 5);

        pbremovefile = new QPushButton(centralWidget);
        pbremovefile->setObjectName(QString::fromUtf8("pbremovefile"));

        gridLayout->addWidget(pbremovefile, 2, 4, 1, 1);

        pbwritefile = new QPushButton(centralWidget);
        pbwritefile->setObjectName(QString::fromUtf8("pbwritefile"));

        gridLayout->addWidget(pbwritefile, 2, 3, 1, 1);

        pbspectrum = new QPushButton(centralWidget);
        pbspectrum->setObjectName(QString::fromUtf8("pbspectrum"));

        gridLayout->addWidget(pbspectrum, 2, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 768, 20));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pbclean->setText(QCoreApplication::translate("MainWindow", "Clean", nullptr));
        pbtime->setText(QCoreApplication::translate("MainWindow", "Time", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "\320\235\320\265\321\201\321\203\321\211\320\260\321\217", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "\320\247\320\260\321\201\321\202\320\276\321\202\320\275\320\260\321\217 \321\202\320\265\320\273\320\265\320\263\321\200\320\260\321\204\320\270\321\217", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MainWindow", "8-\321\204\320\260\320\267\320\276\320\262\320\260\321\217 \320\274\320\276\320\264\321\203\320\273\321\217\321\206\320\270\321\217", nullptr));

        pbclose->setText(QCoreApplication::translate("MainWindow", "Close", nullptr));
        pbremovefile->setText(QCoreApplication::translate("MainWindow", "Remove file", nullptr));
        pbwritefile->setText(QCoreApplication::translate("MainWindow", "Write file", nullptr));
        pbspectrum->setText(QCoreApplication::translate("MainWindow", "Spectrum", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

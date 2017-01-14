/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Nov 27 15:24:54 2015
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionImport;
    QAction *actionSave;
    QAction *actionSaveTexture;
    QAction *actionCoordidateSystem;
    QAction *actionSymmetryAxis;
    QAction *actionStartUnfold;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuOptions;
    QToolBar *mainToolBar;
    QAction *actionImport1;
    QAction *actionSave1;
    QAction *actionSaveTexture1;
    QAction *actionCoordidateSystem1;
    QAction *actionSymmetryAxis1;
    QAction *actionStartUnfold1;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(800, 500);
        actionImport = new QAction(MainWindowClass);
        actionImport->setObjectName(QString::fromUtf8("actionImport"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("Icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImport->setIcon(icon);
        actionSave = new QAction(MainWindowClass);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("Icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        actionSaveTexture = new QAction(MainWindowClass);
        actionSaveTexture->setObjectName(QString::fromUtf8("actionSaveTexture"));
        actionSaveTexture->setIcon(icon1);
        actionCoordidateSystem = new QAction(MainWindowClass);
        actionCoordidateSystem->setObjectName(QString::fromUtf8("actionCoordidateSystem"));
        actionCoordidateSystem->setCheckable(true);
        actionCoordidateSystem->setChecked(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("Icons/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCoordidateSystem->setIcon(icon2);
        actionSymmetryAxis = new QAction(MainWindowClass);
        actionSymmetryAxis->setObjectName(QString::fromUtf8("actionSymmetryAxis"));
        actionSymmetryAxis->setCheckable(true);
        actionSymmetryAxis->setChecked(false);
        actionSymmetryAxis->setIcon(icon2);
        actionStartUnfold = new QAction(MainWindowClass);
        actionStartUnfold->setObjectName(QString::fromUtf8("actionStartUnfold"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("Icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStartUnfold->setIcon(icon3);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setBaseSize(QSize(11, 11));
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        actionImport1 = new QAction(mainToolBar);
        actionImport1->setObjectName(QString::fromUtf8("actionImport1"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("Icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImport1->setIcon(icon4);
        actionSave1 = new QAction(mainToolBar);
        actionSave1->setObjectName(QString::fromUtf8("actionSave1"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("Icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave1->setIcon(icon5);
        actionSaveTexture1 = new QAction(mainToolBar);
        actionSaveTexture1->setObjectName(QString::fromUtf8("actionSaveTexture1"));
        actionSaveTexture1->setIcon(icon5);
        actionCoordidateSystem1 = new QAction(mainToolBar);
        actionCoordidateSystem1->setObjectName(QString::fromUtf8("actionCoordidateSystem1"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("Icons/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCoordidateSystem1->setIcon(icon6);
        actionSymmetryAxis1 = new QAction(mainToolBar);
        actionSymmetryAxis1->setObjectName(QString::fromUtf8("actionSymmetryAxis1"));
        actionSymmetryAxis1->setIcon(icon6);
        actionStartUnfold1 = new QAction(mainToolBar);
        actionStartUnfold1->setObjectName(QString::fromUtf8("actionStartUnfold1"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8("Icons/start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStartUnfold1->setIcon(icon7);
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuFile->addAction(actionImport);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveTexture);
        menuOptions->addAction(actionCoordidateSystem);
        menuOptions->addAction(actionSymmetryAxis);
        menuOptions->addAction(actionStartUnfold);
        mainToolBar->addAction(actionImport);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSaveTexture);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionCoordidateSystem);
        mainToolBar->addAction(actionSymmetryAxis);
        mainToolBar->addAction(actionStartUnfold);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "\350\200\203\345\217\244\347\273\230\345\233\276\350\276\205\345\212\251", 0, QApplication::UnicodeUTF8));
        actionImport->setText(QApplication::translate("MainWindowClass", "\346\211\223\345\274\200", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindowClass", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        actionSaveTexture->setText(QApplication::translate("MainWindowClass", "\344\277\235\345\255\230\347\272\271\347\220\206", 0, QApplication::UnicodeUTF8));
        actionCoordidateSystem->setText(QApplication::translate("MainWindowClass", "\345\235\220\346\240\207\347\263\273", 0, QApplication::UnicodeUTF8));
        actionSymmetryAxis->setText(QApplication::translate("MainWindowClass", "\345\257\271\347\247\260\350\275\264", 0, QApplication::UnicodeUTF8));
        actionStartUnfold->setText(QApplication::translate("MainWindowClass", "\345\274\200\345\247\213", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "\346\226\207\344\273\266", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("MainWindowClass", "\351\200\211\351\241\271", 0, QApplication::UnicodeUTF8));
        actionImport1->setText(QApplication::translate("MainWindowClass", "\346\211\223\345\274\200", 0, QApplication::UnicodeUTF8));
        actionSave1->setText(QApplication::translate("MainWindowClass", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        actionSaveTexture1->setText(QApplication::translate("MainWindowClass", "\344\277\235\345\255\230\347\272\271\347\220\206", 0, QApplication::UnicodeUTF8));
        actionCoordidateSystem1->setText(QApplication::translate("MainWindowClass", "\345\235\220\346\240\207\347\263\273", 0, QApplication::UnicodeUTF8));
        actionSymmetryAxis1->setText(QApplication::translate("MainWindowClass", "\345\257\271\347\247\260\350\275\264", 0, QApplication::UnicodeUTF8));
        actionStartUnfold1->setText(QApplication::translate("MainWindowClass", "\345\274\200\345\247\213", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

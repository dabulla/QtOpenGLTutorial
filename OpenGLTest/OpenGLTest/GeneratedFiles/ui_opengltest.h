/********************************************************************************
** Form generated from reading UI file 'opengltest.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGLTEST_H
#define UI_OPENGLTEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OpenGLTestClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OpenGLTestClass)
    {
        if (OpenGLTestClass->objectName().isEmpty())
            OpenGLTestClass->setObjectName(QStringLiteral("OpenGLTestClass"));
        OpenGLTestClass->resize(600, 400);
        menuBar = new QMenuBar(OpenGLTestClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        OpenGLTestClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(OpenGLTestClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OpenGLTestClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(OpenGLTestClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        OpenGLTestClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(OpenGLTestClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        OpenGLTestClass->setStatusBar(statusBar);

        retranslateUi(OpenGLTestClass);

        QMetaObject::connectSlotsByName(OpenGLTestClass);
    } // setupUi

    void retranslateUi(QMainWindow *OpenGLTestClass)
    {
        OpenGLTestClass->setWindowTitle(QApplication::translate("OpenGLTestClass", "OpenGLTest", 0));
    } // retranslateUi

};

namespace Ui {
    class OpenGLTestClass: public Ui_OpenGLTestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGLTEST_H

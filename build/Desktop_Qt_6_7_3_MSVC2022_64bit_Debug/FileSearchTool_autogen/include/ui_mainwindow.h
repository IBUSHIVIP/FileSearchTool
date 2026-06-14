/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actioncancel;
    QAction *actionhouzhui;
    QAction *actionname;
    QAction *actionclear_filter;
    QAction *actionfilter;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLineEdit *edit_path;
    QPushButton *btn_selectDir;
    QTableWidget *table_result;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_start;
    QPushButton *btn_stop;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1220, 783);
        actioncancel = new QAction(MainWindow);
        actioncancel->setObjectName("actioncancel");
        actionhouzhui = new QAction(MainWindow);
        actionhouzhui->setObjectName("actionhouzhui");
        actionname = new QAction(MainWindow);
        actionname->setObjectName("actionname");
        actionclear_filter = new QAction(MainWindow);
        actionclear_filter->setObjectName("actionclear_filter");
        actionfilter = new QAction(MainWindow);
        actionfilter->setObjectName("actionfilter");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName("widget_2");
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(widget_2);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        edit_path = new QLineEdit(widget);
        edit_path->setObjectName("edit_path");

        horizontalLayout->addWidget(edit_path);

        btn_selectDir = new QPushButton(widget);
        btn_selectDir->setObjectName("btn_selectDir");

        horizontalLayout->addWidget(btn_selectDir);


        verticalLayout->addWidget(widget);

        table_result = new QTableWidget(widget_2);
        if (table_result->columnCount() < 2)
            table_result->setColumnCount(2);
        table_result->setObjectName("table_result");
        table_result->setColumnCount(2);

        verticalLayout->addWidget(table_result);


        verticalLayout_2->addWidget(widget_2);

        widget_3 = new QWidget(centralwidget);
        widget_3->setObjectName("widget_3");
        horizontalLayout_2 = new QHBoxLayout(widget_3);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        btn_start = new QPushButton(widget_3);
        btn_start->setObjectName("btn_start");

        horizontalLayout_2->addWidget(btn_start);

        btn_stop = new QPushButton(widget_3);
        btn_stop->setObjectName("btn_stop");

        horizontalLayout_2->addWidget(btn_stop);


        verticalLayout_2->addWidget(widget_3);


        verticalLayout_3->addLayout(verticalLayout_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1220, 21));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName("menu_2");
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menu->addAction(actioncancel);
        menu->addSeparator();
        menu_2->addAction(actionfilter);
        menu_2->addSeparator();
        menu_2->addAction(actionclear_filter);
        menu_2->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actioncancel->setText(QCoreApplication::translate("MainWindow", "\345\217\226\346\266\210", nullptr));
        actionhouzhui->setText(QCoreApplication::translate("MainWindow", ".\345\220\216\347\274\200", nullptr));
        actionname->setText(QCoreApplication::translate("MainWindow", "\345\220\215\347\247\260\350\277\207\346\273\244", nullptr));
#if QT_CONFIG(tooltip)
        actionname->setToolTip(QCoreApplication::translate("MainWindow", "\345\220\215\347\247\260\350\277\207\346\273\244", nullptr));
#endif // QT_CONFIG(tooltip)
        actionclear_filter->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\350\277\207\346\273\244", nullptr));
        actionfilter->setText(QCoreApplication::translate("MainWindow", "\350\277\207\346\273\244\347\225\214\351\235\242", nullptr));
        btn_selectDir->setText(QCoreApplication::translate("MainWindow", "selectDir", nullptr));
        btn_start->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        btn_stop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\350\277\207\346\273\244\345\231\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

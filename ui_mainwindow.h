/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QFrame *line;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *connectButton;
    QCheckBox *CheckBox_SerialStatus;
    QPushButton *pushButton_2;
    QComboBox *Serial_item;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboBox_2;
    QComboBox *comboBox_3;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_13;
    QLabel *label_11;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_12;
    QVBoxLayout *verticalLayout_6;
    QLineEdit *lineEdit_9;
    QLineEdit *lineEdit_8;
    QLineEdit *lineEdit_7;
    QLineEdit *lineEdit_10;
    QLineEdit *lineEdit_6;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(534, 377);
        MainWindow->setStyleSheet(QStringLiteral("color: rgb(84, 0, 126);"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(9, 155, 503, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(304, 9, 206, 57));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        connectButton = new QPushButton(layoutWidget);
        connectButton->setObjectName(QStringLiteral("connectButton"));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(10);
        connectButton->setFont(font);

        horizontalLayout->addWidget(connectButton);

        CheckBox_SerialStatus = new QCheckBox(layoutWidget);
        CheckBox_SerialStatus->setObjectName(QStringLiteral("CheckBox_SerialStatus"));
        CheckBox_SerialStatus->setMinimumSize(QSize(0, 0));
        CheckBox_SerialStatus->setMaximumSize(QSize(24, 16777215));
        CheckBox_SerialStatus->setStyleSheet(QLatin1String("QCheckBox {spacing: 0px;}\n"
"QCheckBox::indicator {width: 24px;height: 24px;} \n"
"QCheckBox::indicator:checked  {image: url(:/image/checkbox_checked.png);}\n"
"QCheckBox::indicator:unchecked {image: url(:/image/checkbox_unchecked.png);} "));
        CheckBox_SerialStatus->setCheckable(true);

        horizontalLayout->addWidget(CheckBox_SerialStatus);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setFont(font);

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout);

        Serial_item = new QComboBox(layoutWidget);
        Serial_item->setObjectName(QStringLiteral("Serial_item"));
        Serial_item->setMinimumSize(QSize(0, 0));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(9);
        Serial_item->setFont(font1);

        verticalLayout->addWidget(Serial_item);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(9, 10, 255, 152));
        verticalLayout_4 = new QVBoxLayout(layoutWidget1);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        comboBox_2 = new QComboBox(layoutWidget1);
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setFont(font);

        horizontalLayout_2->addWidget(comboBox_2);

        comboBox_3 = new QComboBox(layoutWidget1);
        comboBox_3->addItem(QString());
        comboBox_3->addItem(QString());
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));
        comboBox_3->setFont(font);

        horizontalLayout_2->addWidget(comboBox_3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(layoutWidget1);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        verticalLayout_3->addWidget(label);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);

        verticalLayout_3->addWidget(label_2);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);

        verticalLayout_3->addWidget(label_3);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);

        verticalLayout_3->addWidget(label_4);


        horizontalLayout_3->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        lineEdit = new QLineEdit(layoutWidget1);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMinimumSize(QSize(165, 0));
        lineEdit->setFont(font);

        verticalLayout_2->addWidget(lineEdit);

        lineEdit_2 = new QLineEdit(layoutWidget1);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setFont(font);

        verticalLayout_2->addWidget(lineEdit_2);

        lineEdit_3 = new QLineEdit(layoutWidget1);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setFont(font);

        verticalLayout_2->addWidget(lineEdit_3);

        lineEdit_4 = new QLineEdit(layoutWidget1);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setFont(font);

        verticalLayout_2->addWidget(lineEdit_4);


        horizontalLayout_3->addLayout(verticalLayout_2);


        verticalLayout_4->addLayout(horizontalLayout_3);

        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(9, 164, 253, 148));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_13 = new QLabel(layoutWidget2);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setFont(font);

        verticalLayout_5->addWidget(label_13);

        label_11 = new QLabel(layoutWidget2);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setFont(font);

        verticalLayout_5->addWidget(label_11);

        label_9 = new QLabel(layoutWidget2);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setFont(font);

        verticalLayout_5->addWidget(label_9);

        label_10 = new QLabel(layoutWidget2);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setFont(font);

        verticalLayout_5->addWidget(label_10);

        label_12 = new QLabel(layoutWidget2);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setFont(font);

        verticalLayout_5->addWidget(label_12);


        horizontalLayout_4->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        lineEdit_9 = new QLineEdit(layoutWidget2);
        lineEdit_9->setObjectName(QStringLiteral("lineEdit_9"));
        lineEdit_9->setMinimumSize(QSize(165, 0));
        lineEdit_9->setMaximumSize(QSize(165, 16777215));
        lineEdit_9->setFont(font);
        lineEdit_9->setReadOnly(true);

        verticalLayout_6->addWidget(lineEdit_9);

        lineEdit_8 = new QLineEdit(layoutWidget2);
        lineEdit_8->setObjectName(QStringLiteral("lineEdit_8"));
        lineEdit_8->setMinimumSize(QSize(165, 0));
        lineEdit_8->setMaximumSize(QSize(165, 16777215));
        lineEdit_8->setFont(font);
        lineEdit_8->setReadOnly(true);

        verticalLayout_6->addWidget(lineEdit_8);

        lineEdit_7 = new QLineEdit(layoutWidget2);
        lineEdit_7->setObjectName(QStringLiteral("lineEdit_7"));
        lineEdit_7->setMinimumSize(QSize(165, 0));
        lineEdit_7->setMaximumSize(QSize(165, 16777215));
        lineEdit_7->setFont(font);
        lineEdit_7->setReadOnly(true);

        verticalLayout_6->addWidget(lineEdit_7);

        lineEdit_10 = new QLineEdit(layoutWidget2);
        lineEdit_10->setObjectName(QStringLiteral("lineEdit_10"));
        lineEdit_10->setMinimumSize(QSize(165, 0));
        lineEdit_10->setMaximumSize(QSize(165, 16777215));
        lineEdit_10->setFont(font);
        lineEdit_10->setReadOnly(true);

        verticalLayout_6->addWidget(lineEdit_10);

        lineEdit_6 = new QLineEdit(layoutWidget2);
        lineEdit_6->setObjectName(QStringLiteral("lineEdit_6"));
        lineEdit_6->setMinimumSize(QSize(165, 0));
        lineEdit_6->setMaximumSize(QSize(165, 16777215));
        lineEdit_6->setFont(font);
        lineEdit_6->setReadOnly(true);

        verticalLayout_6->addWidget(lineEdit_6);


        horizontalLayout_4->addLayout(verticalLayout_6);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 534, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setSizeGripEnabled(false);
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TQ_SN", nullptr));
        connectButton->setText(QApplication::translate("MainWindow", "Connect", nullptr));
        CheckBox_SerialStatus->setText(QString());
        pushButton_2->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        comboBox_2->setItemText(0, QApplication::translate("MainWindow", "AWG", nullptr));

        comboBox_3->setItemText(0, QApplication::translate("MainWindow", "L28 ", nullptr));
        comboBox_3->setItemText(1, QApplication::translate("MainWindow", "L50R", nullptr));

        label->setText(QApplication::translate("MainWindow", "DeviceName", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "ProductKey", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "DeviceSecret", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "ClientID", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "WriteFlag", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "DeviceName", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "ProductKey", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "DeviceSecret", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "ClientID", nullptr));
        lineEdit_9->setText(QString());
        lineEdit_8->setText(QApplication::translate("MainWindow", "AWG-XXXX-XXXXXX-XXXXXX", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

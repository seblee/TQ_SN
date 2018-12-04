#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QDir::setCurrent(a.applicationDirPath()); //添加这句 立马可以

    MainWindow w;
    w.show();

    return a.exec();
}

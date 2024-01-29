#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    w.setStyleSheet("#MainWindow{background-color:lightblue;}");
    //w.setStyleSheet("QMainWindow::windowTitle{background-color:lightblue;}");
    return a.exec();
}

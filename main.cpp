#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator chTranslator;
    chTranslator.load(":/trans/zh/zh_CN.qm");
    app.installTranslator(&chTranslator);
    MainWindow w;
    w.show();

    return app.exec();
}

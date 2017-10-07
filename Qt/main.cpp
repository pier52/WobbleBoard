#include "mainwindow.h"
#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[])
{
    // Force to always be in English locale, otherwise parsing and writing ASCII logs will be a headache
    // (decimal points are for example a dot in English, but a comma in Swedish)
    QLocale::setDefault(QLocale(QLocale::English,QLocale::UnitedStates));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

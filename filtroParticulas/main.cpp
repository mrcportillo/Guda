#include "mainwindow.h"
#include <QApplication>
#include "detectarvehiculo2.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.move(0,300);
    w.show();
    DetectarVehiculo2 d; //Se ejecuta porque esta en el constructor
    d.comenzar();

    return a.exec();
}

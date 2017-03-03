#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "detectarvehiculo2.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    DetectarVehiculo2::actualizarZoom(arg1);
}

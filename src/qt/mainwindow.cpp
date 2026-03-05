#include "mainwindow.hpp"

mainwindow::mainwindow(QWidget *parent) :
QMainWindow(parent) {
    this->setWindowTitle("FileFlow");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(WIDTH, HEIGHT);
}

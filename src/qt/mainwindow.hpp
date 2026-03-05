#pragma once

#define WIDTH 800
#define HEIGHT 600

#include <QMainWindow>

class mainwindow : public QMainWindow{
    Q_OBJECT
    public:
    mainwindow(QWidget* parent=nullptr);
};
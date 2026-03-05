#include  <QApplication>

#include "qt/mainwindow.hpp"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    mainwindow w;
    w.setWindowIcon(QIcon(":/icons/app_icons.png"));
    w.show();

    return app.exec();
}
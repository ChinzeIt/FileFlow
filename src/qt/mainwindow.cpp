#include "mainwindow.hpp"

mainwindow::mainwindow(QWidget *parent) :
QMainWindow(parent) {
    this->setWindowTitle("FileFlow");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(WIDTH, HEIGHT);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - WIDTH) / 2;
    int y = (screenGeometry.height() - HEIGHT) / 2;
    this->move(x, y);

    setUpUI();
    setConnections();
}

void mainwindow::setUpUI() {
    mainWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(10);

    labelFrom = new QLabel("From place:", this);

    textLineFrom = new QLineEdit(this);

    labelTo = new QLabel("To place:", this);

    textLineTo = new QLineEdit(this);

    labelFormat = new QLabel("Format:", this);

    textLineOther = new QLineEdit(this);

    buttonStart = new QPushButton("Start", this);

    mainLayout->addWidget(labelFrom);
    mainLayout->addWidget(textLineFrom);
    mainLayout->addWidget(labelTo);
    mainLayout->addWidget(textLineTo);
    mainLayout->addWidget(labelFormat);
    mainLayout->addWidget(textLineOther);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonStart);

    setCentralWidget(mainWidget);
}

void mainwindow::setConnections() {
    connect(buttonStart, &QPushButton::clicked, this, &mainwindow::onStartButton);
}

void mainwindow::onStartButton() {
    moving.findListFiles(textLineFrom->text().toStdString(), textLineOther->text().toStdString());
    moving.moveFiles(textLineTo->text().toStdString());
}

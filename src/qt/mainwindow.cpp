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
    this->setFocus();
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

    labelFormat = new QLabel("Format file:", this);

    textLineOther = new QLineEdit(this);

    buttonStart = new QPushButton("START", this);
    buttonStart->setEnabled(false);
    buttonStart->setStyleSheet("QPushButton { background-color: red; }");

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
    connect(textLineFrom, &QLineEdit::textChanged, this, &mainwindow::onTextLineFrom);
    connect(textLineTo, &QLineEdit::textChanged, this, &mainwindow::onTextLineTo);
}

void mainwindow::onTextLineFrom() {
    try {
        pathChecker(textLineFrom->text().toStdString()).exits().isDirectory().isLink().isKnown().isReadable();
        qDebug() << "Succes";
    } catch (const std::exception &e) {
        qDebug() << e.what();
    }
}

void mainwindow::onTextLineTo() {
    try {
        pathChecker(textLineTo->text().toStdString()).exits().isDirectory().isLink().isKnown().isWritable();
        qDebug() << "Succes";
    } catch (const std::exception &e) {
        qDebug() << e.what();
    }
}

void mainwindow::onStartButton() {
    movingFiles::findAndMoveFiles(textLineFrom->text().toStdString(),
        textLineTo->text().toStdString(),
        textLineOther->text().toStdString());
}

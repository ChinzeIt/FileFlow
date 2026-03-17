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

    labelErrorFrom = new QLabel(this);
    labelErrorFrom->setStyleSheet("font-size: 10pt; color: red;");
    labelErrorFrom->hide();

    labelTo = new QLabel("To place:", this);

    textLineTo = new QLineEdit(this);

    labelErrorTo = new QLabel(this);
    labelErrorTo->setStyleSheet("font-size: 10pt; color: red;");
    labelErrorTo->hide();

    labelFormat = new QLabel("Format file:", this);

    textLineOther = new QLineEdit(this);

    buttonStart = new QPushButton("START", this);
    buttonStart->setEnabled(false);
    buttonStart->setStyleSheet("QPushButton { background-color: red; }");

    mainLayout->addWidget(labelFrom);
    mainLayout->addWidget(textLineFrom);
    mainLayout->addWidget(labelErrorFrom);
    mainLayout->addWidget(labelTo);
    mainLayout->addWidget(textLineTo);
    mainLayout->addWidget(labelErrorTo);
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
        labelErrorFrom->hide();
        m_validFrom = true;
    } catch (const std::exception &e) {
        labelErrorFrom->setText("Error: " + QString(e.what()));
        labelErrorFrom->show();
        m_validFrom = false;
    }
    updateStartButton();
}

void mainwindow::onTextLineTo() {
    try {
        pathChecker(textLineTo->text().toStdString()).exits().isDirectory().isLink().isKnown().isWritable();
        labelErrorTo->hide();
        m_validTo = true;
    } catch (const std::exception &e) {
        labelErrorTo->setText("Error: " + QString(e.what()));
        labelErrorTo->show();
        m_validTo = false;
    }
    updateStartButton();
}

void mainwindow::updateStartButton() {
    buttonStart->setEnabled(m_validFrom && m_validTo);
    buttonStart->setStyleSheet(m_validFrom && m_validTo
        ? "QPushButton { background-color: green; }"
        : "QPushButton { background-color: red; }");
}

void mainwindow::onStartButton() {
    movingFiles::findAndMoveFiles(textLineFrom->text().toStdString(),
        textLineTo->text().toStdString(),
        textLineOther->text().toStdString());
}

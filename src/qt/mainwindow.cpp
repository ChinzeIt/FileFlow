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

    formatSelector = new FileFormatSelector(this);

    buttonStart = new QPushButton("START", this);
    buttonStart->setEnabled(false);
    buttonStart->setStyleSheet("QPushButton { background-color: red; }");

    labelErrorPaths = new QLabel("Line from and second identical", this);
    labelErrorPaths->setStyleSheet("font-size: 10pt; color: red;");
    labelErrorPaths->hide();

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFixedHeight(20);
    progressBar->setGeometry(0, HEIGHT - 20, WIDTH, 20);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: none;
            border-radius: 0px;
            background-color: transparent;
        }
        QProgressBar::chunk {
            background-color: #00e676;
            border-radius: 0px;
        }
    )");
    progressBar->hide();

    mainLayout->addWidget(labelFrom);
    mainLayout->addWidget(textLineFrom);
    mainLayout->addWidget(labelErrorFrom);
    mainLayout->addWidget(labelTo);
    mainLayout->addWidget(textLineTo);
    mainLayout->addWidget(labelErrorTo);
    mainLayout->addWidget(formatSelector, 1);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonStart);
    mainLayout->addWidget(labelErrorPaths);

    setCentralWidget(mainWidget);
}

void mainwindow::setConnections() {
    connect(buttonStart, &QPushButton::clicked, this, &mainwindow::onStartButton);
    connect(textLineFrom, &QLineEdit::textChanged, this, &mainwindow::onTextLineFrom);
    connect(textLineTo, &QLineEdit::textChanged, this, &mainwindow::onTextLineTo);
    connect(formatSelector, &FileFormatSelector::selectionChanged, this, [this](bool hasSelection) {
        m_validFormats = hasSelection;
        updateStartButton();
    });
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
    auto normalize = [](const QString &s) -> QString {
        return s.endsWith('/') ? s : s + '/';
    };

    m_validPaths = normalize(textLineFrom->text()) != normalize(textLineTo->text()) ||
                   textLineFrom->text().isEmpty();

    labelErrorPaths->setVisible(!m_validPaths);

    buttonStart->setEnabled(m_validFrom && m_validTo && m_validFormats && m_validPaths);
    buttonStart->setStyleSheet(m_validFrom && m_validTo && m_validFormats && m_validPaths
        ? "QPushButton { background-color: green; }"
        : "QPushButton { background-color: red; }");
}

void mainwindow::onStartButton() {
    offMainUI();

    progressBar->setValue(0);
    progressBar->show();
    controll.start(
        textLineFrom->text().toStdString(),
        textLineTo->text().toStdString(),
        formatSelector->selected(),
        {
            .successFinished = [this] { QMetaObject::invokeMethod(this,
                &mainwindow::successFinished,
                Qt::QueuedConnection);
            },
            .progressWork = [this](int cur, int tot) { QMetaObject::invokeMethod(this, [this, cur, tot] {
                progressBar->setValue(cur * 100 / tot);
            }, Qt::QueuedConnection);
            }
        }
        );
}

void mainwindow::offMainUI() {
    buttonStart->setEnabled(false);
    buttonStart->setStyleSheet("QPushButton { background-color: gray; }");
    textLineFrom->setEnabled(false);
    textLineTo->setEnabled(false);
    formatSelector->setEnabled(false);
}

void mainwindow::onMainUI() {
    textLineFrom->setEnabled(true);
    textLineTo->setEnabled(true);
    formatSelector->setEnabled(true);
    buttonStart->setStyleSheet("QPushButton { background-color: green; }");
    buttonStart->setEnabled(true);
}

void mainwindow::successFinished() {
    progressBar->hide();
    onMainUI();
}

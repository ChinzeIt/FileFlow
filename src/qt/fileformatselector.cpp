#include "fileformatselector.hpp"
#include <algorithm>

FileFormatSelector::FileFormatSelector(QWidget *parent) : QWidget(parent) {
    buildUI();
}

void FileFormatSelector::buildUI() {
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(4);

    QLabel *label = new QLabel("Format file:", this);
    outerLayout->addWidget(label);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollBar:vertical {
            width: 6px;
            background: transparent;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: black;
            border-radius: 0px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
            border: none;
        }
    )");
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(8, 8, 8, 8);
    contentLayout->setSpacing(8);

    m_btnAll = new QPushButton("All", scrollContent);
    m_btnAll->setCheckable(true);
    m_btnAll->setFixedHeight(24);
    m_btnAll->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_btnAll->setStyleSheet(R"(
        QPushButton {
            border: 1px solid #aaa;
            border-radius: 3px;
            padding: 0 10px;
            font-size: 9pt;
        }
        QPushButton:checked {
            background-color: #c0392b;
            color: white;
            border: 1px solid #e74c3c;
        }
        QPushButton:hover:!checked {
            background-color: #e0e0e0;
        }
    )");

    QPushButton *btnClear = new QPushButton("Clear", scrollContent);
    btnClear->setFixedHeight(24);
    btnClear->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    btnClear->setStyleSheet(R"(
    QPushButton {
            background-color: #7b1c1c;
            color: white;
            border: 1px solid #a02020;
            border-radius: 3px;
            padding: 0 10px;
            font-size: 9pt;
        }
        QPushButton:hover {
            background-color: #9b2222;
        }
        QPushButton:pressed {
            background-color: #5a1010;
        }
    )");

    connect(btnClear, &QPushButton::clicked, this, [this]() {
        for (QPushButton *btn : m_allButtons) {
            QSignalBlocker blocker(btn);
            btn->setChecked(false);
        }
        QSignalBlocker blocker(m_btnAll);
        m_btnAll->setChecked(false);
        m_selected.clear();
        emit selectionChanged(!m_selected.empty());
    });

    QHBoxLayout *allRow = new QHBoxLayout();
    allRow->setContentsMargins(0, 0, 0, 4);
    allRow->addWidget(m_btnAll);
    allRow->addWidget(btnClear);
    allRow->addStretch();
    contentLayout->addLayout(allRow);

    addCategory(contentLayout, "Photo",    {"png", "jpg", "jpeg", "gif", "bmp", "webp", "tiff", "tif", "svg", "ico", "heic", "heif", "raw", "cr2", "nef", "arw", "dng", "psd", "xcf", "avif"});
    addCategory(contentLayout, "Video",    {"mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "mpeg", "mpg", "3gp", "ogv", "ts", "mts", "m2ts", "vob", "divx", "rmvb", "asf"});
    addCategory(contentLayout, "Audio",    {"mp3", "wav", "flac", "aac", "ogg", "wma", "m4a", "opus", "aiff", "ape", "mid", "midi", "amr", "ra", "ac3", "dts", "mka", "tta", "wv"});
    addCategory(contentLayout, "Document", {"pdf", "doc", "docx", "xls", "xlsx", "ppt", "pptx", "odt", "ods", "odp", "txt", "rtf", "csv", "md", "tex", "epub", "fb2", "djvu", "pages", "numbers"});
    addCategory(contentLayout, "Archive",  {"zip", "tar", "gz", "7z", "rar", "bz2", "xz", "tar.gz", "tar.bz2", "tar.xz", "zst", "lz4", "cab", "iso", "dmg", "pkg", "deb", "rpm"});
    addCategory(contentLayout, "Code",     {"cpp", "hpp", "c", "h", "py", "js", "ts", "html", "css", "json", "xml", "yaml", "yml", "toml", "ini", "sh", "bat", "ps1", "rs", "go", "java", "kt", "swift", "rb", "php", "lua", "sql", "cmake"});
    addCategory(contentLayout, "3D",       {"obj", "fbx", "stl", "gltf", "glb", "blend", "dae", "3ds", "max", "maya", "ply", "step", "iges", "stp"});
    addCategory(contentLayout, "Font",     {"ttf", "otf", "woff", "woff2", "eot", "fon", "pfb", "afm"});
    addCategory(contentLayout, "Database", {"db", "sqlite", "sqlite3", "mdb", "accdb", "sql", "dump", "bak", "dbf"});
    addCategory(contentLayout, "Disk",     {"iso", "img", "vhd", "vhdx", "vmdk", "qcow2", "vdi", "bin", "nrg", "mdf"});
    addCategory(contentLayout, "Ebook",    {"epub", "fb2", "mobi", "azw", "azw3", "lit", "lrf", "pdb", "djvu"});
    addCategory(contentLayout, "Subtitle", {"srt", "ass", "ssa", "vtt", "sub", "sbv", "idx", "sup"});
    contentLayout->addStretch();

    connect(m_btnAll, &QPushButton::clicked, this, [this](bool checked) {
        for (QPushButton *btn : m_allButtons) {
            QSignalBlocker blocker(btn);
            btn->setChecked(checked);
        }
        m_selected.clear();
        if (checked) {
            for (QPushButton *btn : m_allButtons)
                m_selected.push_back("." + btn->text().toStdString());
        }
        emit selectionChanged(!m_selected.empty());
    });

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void FileFormatSelector::addCategory(QVBoxLayout *layout, const QString &name, const QStringList &formats) {
    QLabel *catLabel = new QLabel(name, this);
    catLabel->setStyleSheet("font-size: 9pt; font-weight: bold;");
    layout->addWidget(catLabel);

    QVBoxLayout *rowsLayout = new QVBoxLayout();
    rowsLayout->setSpacing(4);
    rowsLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *row = nullptr;
    int count = 0;

    for (const QString &fmt : formats) {
        if (count % 5 == 0) {
            row = new QHBoxLayout();
            row->setSpacing(6);
            row->setContentsMargins(0, 0, 0, 0);
            rowsLayout->addLayout(row);
        }

        QPushButton *btn = new QPushButton(fmt, this);
        btn->setCheckable(true);
        btn->setFixedHeight(24);
        btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        btn->setStyleSheet(R"(
            QPushButton {
                border: 1px solid #aaa;
                border-radius: 3px;
                padding: 0 10px;
                font-size: 9pt;
            }
            QPushButton:checked {
                background-color: #c0392b;
                color: white;
                border: 1px solid #e74c3c;
            }
            QPushButton:hover:!checked {
                background-color: #e0e0e0;
            }
        )");

        connect(btn, &QPushButton::toggled, this, [this, fmt](bool checked) {
            std::string ext = "." + fmt.toStdString();
            if (checked) {
                m_selected.push_back(ext);
            } else {
                m_selected.erase(std::remove(m_selected.begin(), m_selected.end(), ext), m_selected.end());
            }

            bool allChecked = std::all_of(m_allButtons.begin(), m_allButtons.end(),
                [](QPushButton *b) { return b->isChecked(); });
            QSignalBlocker blocker(m_btnAll);
            m_btnAll->setChecked(allChecked);
            emit selectionChanged(!m_selected.empty());
        });

        row->addWidget(btn);
        m_allButtons.append(btn);
        count++;

        if (count == formats.size()) {
            row->addStretch();
        }
    }

    layout->addLayout(rowsLayout);
}
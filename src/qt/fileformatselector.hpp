#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QStringList>

class FileFormatSelector : public QWidget {
    Q_OBJECT
public:
    explicit FileFormatSelector(QWidget *parent = nullptr);
    const std::vector<std::string>& selected() const { return m_selected; }

signals:
    void selectionChanged(bool hasSelection);

private:
    void buildUI();
    void addCategory(QVBoxLayout *layout, const QString &name, const QStringList &formats);

    QList<QPushButton*> m_allButtons;
    QPushButton *m_btnAll = nullptr;
    std::vector<std::string> m_selected;
};
#pragma once

#define WIDTH 400
#define HEIGHT 600

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QScreen>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>

#include "../core/utils/pathChecker.hpp"
#include "../controller/controller.hpp"
#include "fileformatselector.hpp"

class mainwindow : public QMainWindow {
    Q_OBJECT
public:
    mainwindow(QWidget* parent=nullptr);
private Q_SLOTS:
    void onStartButton();
private:
    controller controll;
    void setUpUI();
    void offMainUI();
    void onMainUI();
    void setConnections();

    QWidget* mainWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* labelFrom;
    QLineEdit* textLineFrom;
    void onTextLineFrom();
    QLabel* labelErrorFrom;

    QLabel* labelTo;
    QLineEdit* textLineTo;
    void onTextLineTo();
    QLabel* labelErrorTo;

    FileFormatSelector *formatSelector;

    QPushButton* buttonStart;
    void updateStartButton();
    bool m_validFrom = false;
    bool m_validTo = false;
    bool m_validFormats = false;
    bool m_validPaths = true;
    void successFinished();
    QProgressBar *progressBar;

    QLabel *labelErrorPaths;
};
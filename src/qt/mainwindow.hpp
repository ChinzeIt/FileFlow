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

#include "../core/utils/pathChecker.hpp"
#include "../core/movingFiles.hpp"

class mainwindow : public QMainWindow {
    Q_OBJECT
public:
    mainwindow(QWidget* parent=nullptr);
private Q_SLOTS:
    void onStartButton();
private:
    void setUpUI();
    void setConnections();

    QWidget* mainWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* labelFrom;
    QLineEdit* textLineFrom;
    void onTextLineFrom();

    QLabel* labelTo;
    QLineEdit* textLineTo;
    void onTextLineTo();

    QLabel* labelFormat;
    QLineEdit* textLineOther;

    QPushButton* buttonStart;
};
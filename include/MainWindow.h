#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void onRunButtonClicked();

private:
    QPushButton *simulateButton;
    QPushButton *newButton;
    QPushButton *openFileButton;
};
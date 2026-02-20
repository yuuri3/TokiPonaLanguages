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
    void Unimplemented();

private:
    QMenuBar *menuBar;

    QMenu *fileMenu;
    QMenu *simulationMenu;

    QAction *simulateAction;
    QAction *newFileAction;
    QAction *openFileAction;
};
#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include "LanguageFamilySimulator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void Unimplemented();
    void Simulate();

private:
    QMenuBar *menuBar;

    QMenu *fileMenu;
    QMenu *simulationMenu;

    std::optional<LanguageFamilySimulator> simulator;

    QAction *simulateAction;
    QAction *newFileAction;
    QAction *openFileAction;

    void DisplayLanguageFamily();
};
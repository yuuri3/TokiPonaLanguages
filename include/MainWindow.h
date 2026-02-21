#pragma once

#include "LanguageFamilySimulator.h"
#include "stdafx.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void Unimplemented();
    void Simulate();
    void SaveFile();
    void OpenFile();
    void NewFile();

private:
    QMenuBar *menuBar;

    QMenu *fileMenu;
    QMenu *simulationMenu;

    std::optional<LanguageFamilySimulator> simulator;

    QAction *simulateAction;
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *saveFileAction;

    QTableWidget *mainTable;

    void DisplayLanguageFamily();
    void WarningUnsaveFile();
    void DisplayTable(const std::vector<std::vector<std::string>> &data);
};
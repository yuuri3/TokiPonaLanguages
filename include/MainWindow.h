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
    void ShowContextMenu(const QPoint &pos);

private:
    QMenuBar *menuBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *simulationMenu;
    QMenu *helpMenu;

    std::optional<LanguageFamilySimulator> simulator = std::nullopt;

    QAction *simulateAction;
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *phonologicalChangeAction;
    QAction *loanwordAction;
    QAction *helpAction;

    QTableWidget *mainTable;

    void DisplayLanguageFamily();
    void WarningUnsaveFile();
    void DisplayTable(const std::vector<std::vector<std::string>> &data);
    void EditLanguage(const std::string place, const int period);

protected:
    void closeEvent(QCloseEvent *event) override;
};
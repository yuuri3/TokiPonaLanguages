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
    void ShowVersion();
    void ShowQtLicense();

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
    QAction *versionAction;
    QAction *QtAction;

    QTableWidget *mainTable;

    bool isLanguagesSaved;

    void DisplayLanguageFamily();
    void WarningUnsaveFile();
    void EditLanguage(const std::string place, const int period);
    void EditPeriod(const std::string place, const int period);
    void EditGeometry(const std::string place, const int period);

protected:
    void closeEvent(QCloseEvent *event) override;
};
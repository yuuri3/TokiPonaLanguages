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
    QMenuBar *MenuBar_;

    QMenu *FileMenu_;
    QMenu *EditMenu_;
    QMenu *SimulationMenu_;
    QMenu *HelpMenu_;

    std::shared_ptr<LanguageFamily> Languages_;

    QAction *SimulateAction_;
    QAction *NewFileAction_;
    QAction *OpenFileAction_;
    QAction *SaveFileAction_;
    QAction *PhonologicalChangeAction_;
    QAction *LoanwordAction_;
    QAction *EditGeometry_;
    QAction *HelpAction_;
    QAction *VersionAction_;
    QAction *QtAction_;

    QTableWidget *MainTable_;

    bool IsLanguagesSaved_;

    void DisplayLanguageFamily(const std::shared_ptr<LanguageFamily> languages);
    void WarningUnsaveFile();
    void EditLanguage(const std::string place, const int period);
    void EditPeriod(const std::string place, const int period);
    void EditGeometry(const std::string place, const int period);

protected:
    void closeEvent(QCloseEvent *event) override;
};
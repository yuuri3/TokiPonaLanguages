#pragma once

#include "LanguageFamily.h"
#include "stdafx.h"
#include "DialogLayout.h"
#include "TableData.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void Unimplemented();
    void SaveFile();
    void EditPhonologicalChange();
    void EditLoanword();
    void EditGeometryFromMenu();
    void EditPeriodFromMenu();
    void OpenFile();
    void NewFile();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();
    void ShowVersion();
    void ShowQtLicense();

private:
    QMenuBar *MenuBar_;

    QMenu *FileMenu_;
    QMenu *EditMenu_;
    QMenu *HelpMenu_;

    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<TableData> LanguageNames_;

    QAction *NewFileAction_;
    QAction *OpenFileAction_;
    QAction *SaveFileAction_;
    QAction *PhonologicalChangeAction_;
    QAction *LoanwordAction_;
    QAction *EditGeometry_;
    QAction *EditPeriod_;
    QAction *HelpAction_;
    QAction *VersionAction_;
    QAction *QtAction_;

    DialogLayout LayoutData_;
    static constexpr int MAIN_TABLE_ID = 0;

    bool IsLanguagesSaved_;

    void DisplayLanguageFamily(const std::shared_ptr<LanguageFamily> languages);
    bool WarningUnsaveFile();
    void EditLanguage(const int place, const int period);
    void EditPhonologicalChangeWithIndex(const int row, const int column);
    void EditLoanwordWithIndex(const int row, const int column);
    void EditPeriod(const int place, const int period);
    void EditGeometry(const int place, const int period);

protected:
    void closeEvent(QCloseEvent *event) override;
};
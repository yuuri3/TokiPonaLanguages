#pragma once

#include "stdafx.h"
#include "DialogLayout.h"
#include "LanguageFamily.h"
#include "Language.h"
#include "TableData.h"

class EditLoanwordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditLoanwordDialog(QWidget *parent = nullptr);
    ~EditLoanwordDialog() override = default;

    void Set(const std::shared_ptr<LanguageFamily> languages, const TableData &languageNames);

private slots:
    void ShowHelp();
    void SaveButtonClicked();
    void SelectLanguage();
    void SelectReferenceLanguage();
    void SelectWord();
    void BorrowWord();
    void ShowContextMenu(const QPoint &pos);
    void DeleteLoanword();

private:
    DialogLayout layoutData_;

    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<TableData> LanguageNames_;
    std::optional<Language> TargetLanguage_;
    int TargetPlace_;
    int TargetPeriod_;
    std::optional<Language> ReferenceLanguage_;
    int ReferencePlace_;
    int ReferencePeriod_;
    int SelectedWordID_;
    int TargetWordID_;
    // 借用元単語ID, 借用先単語ID
    std::vector<std::pair<int, int>> LoanwordIDs_;

    void DisplayLoanword();
};
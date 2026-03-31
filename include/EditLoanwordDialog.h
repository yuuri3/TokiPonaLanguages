#pragma once

#include "stdafx.h"
#include "DialogLayout.h"
#include "LanguageFamily.h"
#include "Language.h"

class EditLoanwordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditLoanwordDialog(QWidget *parent = nullptr);
    ~EditLoanwordDialog() override = default;

    void SetLanguages(const std::shared_ptr<LanguageFamily> languages);
    void SetLanguageNames(const std::vector<std::vector<std::string>> languageNames);

private slots:
    void ShowHelp();
    void SaveButtonClicked();
    void SelectLanguage();
    void SelectReferenceLanguage();
    void SelectWord();
    void BorrowWord();

private:
    DialogLayout layoutData_;

    std::shared_ptr<LanguageFamily> languages_;
    std::optional<std::vector<std::vector<std::string>>> languageNames_;
    std::string TargetPlace_;
    int TargetPeriod_;
    std::string ReferencePlace_;
    int ReferencePeriod_;
};
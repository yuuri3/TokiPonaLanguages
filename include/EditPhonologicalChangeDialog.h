#pragma once

#include "stdafx.h"
#include "DialogLayout.h"
#include "LanguageFamily.h"
#include "Language.h"

class EditPhonologicalChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPhonologicalChangeDialog(QWidget *parent = nullptr);
    ~EditPhonologicalChangeDialog() override = default;

    void SetLanguages(const std::shared_ptr<LanguageFamily> languages);
    void SetLanguageNames(const std::vector<std::vector<std::string>> languageNames);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();
    void SelectLanguageName();
    void AddPhonologicalChange();
    void MoveRuleUp();
    void MoveRuleDown();
    void DeleteRule();

private:
    DialogLayout LayoutData_;

    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<std::vector<std::vector<std::string>>> LanguageNames_;
    Language Language_;
};
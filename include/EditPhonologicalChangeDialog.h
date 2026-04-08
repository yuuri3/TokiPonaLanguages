#pragma once

#include "stdafx.h"
#include "DialogLayout.h"
#include "LanguageFamily.h"
#include "Language.h"
#include "TableData.h"

class EditPhonologicalChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPhonologicalChangeDialog(QWidget *parent = nullptr);
    ~EditPhonologicalChangeDialog() override = default;

    void SetLanguages(const std::shared_ptr<LanguageFamily> languages);
    void SetLanguageNames(const TableData &languageNames);
    void SetPlaceAndPeriod(const int row, const int column);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();
    void OKButtonClicked();
    void SelectLanguageName();
    void AddPhonologicalChange();
    void MoveRuleUp();
    void MoveRuleDown();
    void DeleteRule();

private:
    DialogLayout LayoutData_;

    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<TableData> LanguageNames_;
    std::string Place_;
    int Period_;

    void DisplayPhonologicalChanges(const std::string place, const int period);
};
#pragma once

#include "stdafx.h"
#include "Language.h"
#include "DialogLayout.h"
#include "LanguageFamily.h"

class SelectWordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectWordDialog(QWidget *parent = nullptr);
    ~SelectWordDialog() override = default;

    void SetLanguageFamily(std::shared_ptr<LanguageFamily> languageFamily);
    void SetLanguage(std::shared_ptr<Language> targetLanguage, int *outSelectedWordId);

public slots:
    void Unimplemented();
    void SearchWord();
    void SelectWord(int row, int column);
    void OKButtonPushed();

private:
    DialogLayout LayoutData_;
    std::shared_ptr<LanguageFamily> LanguageFamily_;
    std::shared_ptr<Language> Language_;
    int *SelectedWordId_;
    int CurrentSelectedWordId_;
};
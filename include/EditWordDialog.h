#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"
#include "Language.h"
#include "DialogLayout.h"

class EditWordDialog : public QDialog
{
    Q_OBJECT

public:
    EditWordDialog(QWidget *parent = nullptr);
    ~EditWordDialog() = default;
    void Set(std::shared_ptr<LanguageFamily> languages,
             const Language &language,
             const std::string &place,
             const int period,
             const int wordID);

private slots:
    void AddTranslationButtonPushed();
    void ClickLine(const int id, const QPoint &pos);
    void AddTagsButtonPushed();
    void AddContentsButtonPushed();
    void AddVariationsButtonPushed();
    void AddRelationsButtonPushed();
    void Unimplemented();

private:
    const std::vector<int> TWO_WIDTHS = {50, 200};
    const std::vector<int> ONE_WIDTH = {50};
    const int BUTTON_WIDTH = 50;

    DialogLayout LayoutData_;

    std::shared_ptr<LanguageFamily> Languages_;
    std::string Place_;
    int Period_;
    int WordID_;
    Language Language_;

    void UpdateDialog();
    void AddLine(const int id, const std::vector<std::string> &values, const std::vector<int> &widths);
    void DisplayLine(const int id, const std::vector<std::vector<std::string>> &values, const std::vector<int> &widths);
};
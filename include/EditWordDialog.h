#pragma once

#include "stdafx.h"
#include "Word.h"
#include "PhonemeConverter.h"
#include "LanguageFamily.h"

class EditWordDialog : public QDialog
{
    Q_OBJECT

public:
    EditWordDialog(QWidget *parent = nullptr);
    ~EditWordDialog() = default;
    void Set(const LanguageFamily &languages,
             const std::string &place,
             const int period,
             const int wordID);
    void SetLanguage(const Language &language);

private slots:
    void AddTranslationButtonPushed();
    void ClickLine(const QPoint &pos);
    void AddTagsButtonPushed();
    void AddContentsButtonPushed();
    void AddVariationsButtonPushed();
    void AddRelationsButtonPushed();
    void Unimplemented();

private:
    const std::vector<int> TWO_WIDTHS = {50, 200};
    const std::vector<int> ONE_WIDTH = {50};
    const int BUTTON_WIDTH = 50;

    QLineEdit *Entry;
    QWidget *Translations;
    QPushButton *AddTranslationButton;
    QWidget *Tags;
    QPushButton *AddTagsButton;
    QWidget *Contents;
    QPushButton *AddContentsButton;
    QWidget *Variations;
    QPushButton *AddVariationsButton;
    QWidget *Relations;
    QPushButton *AddRelationsButton;

    std::optional<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;
    std::optional<int> WordID;
    std::optional<Language> Language_;

    void UpdateDialog();
    void AddLine(QWidget *widget, const std::vector<std::string> &values, const std::vector<int> &widths);
    void DisplayLine(QWidget *widget, const std::vector<std::vector<std::string>> &values, const std::vector<int> &widths);
};
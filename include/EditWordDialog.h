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
    void Set(std::shared_ptr<LanguageFamily> languages,
             std::optional<std::string> place,
             std::optional<int> period,
             std::optional<int> wordID);

private slots:
    void AddTranslationButtonPushed();
    void ClickLine(const QPoint &pos);
    void AddTagsButtonPushed();
    void AddContentsButtonPushed();
    void AddVariationsButtonPushed();
    void AddRelationsButtonPushed();
    void Unimplemented();

private:
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

    std::shared_ptr<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;
    std::optional<int> WordID;

    void UpdateDialog();
    void AddLine(QWidget *widget, const std::vector<std::string> &value);
    void DisplayLine(QWidget *widget, const std::vector<std::vector<std::string>> &values);
};
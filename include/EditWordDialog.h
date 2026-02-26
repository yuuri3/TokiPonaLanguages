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
    void Click1Col(const QPoint &pos);
    void Click2Cols(const QPoint &pos);
    void AddTagsButtonPushed();
    void AddContentsButtonPushed();
    void Unimplemented();

private:
    QLineEdit *Entry;
    QWidget *Translations;
    QPushButton *AddTranslationButton;
    QWidget *Tags;
    QPushButton *AddTagsButton;
    QWidget *Contents;
    QPushButton *AddContentsButton;
    QTableWidget *Variations;
    QTableWidget *Relations;

    std::shared_ptr<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;
    std::optional<int> WordID;

    void UpdateDialog();
    void Add1Col(QWidget *widget, std::string value);
    void Add2Cols(QWidget *widget, std::string title, std::string value);
    void Display1Col(QWidget *widget, std::vector<std::string> values);
    void Display2Cols(QWidget *widget, const std::vector<std::pair<std::string, std::string>> &translations);
};
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
    void Unimplemented();

private:
    QLineEdit *Entry;
    QVBoxLayout *TranslationLayout;
    QTableWidget *Tags;
    QTableWidget *Contents;
    QTableWidget *Variations;
    QTableWidget *Relations;

    std::shared_ptr<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;
    std::optional<int> WordID;

    void UpdateDialog();
    void DisplayTranslations(QVBoxLayout *layout, const std::vector<std::pair<std::string, std::string>> &translations);
};
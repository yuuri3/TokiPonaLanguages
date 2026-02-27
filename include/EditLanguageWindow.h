#pragma once

#include "stdafx.h"
#include "LanguageFamilySimulator.h"

class EditLanguageWindow : public QDialog
{
    Q_OBJECT

public:
    EditLanguageWindow(QWidget *parent = nullptr);
    ~EditLanguageWindow() = default;
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);

private:
    std::shared_ptr<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;
    std::optional<Language> Language_;

    QTableWidget *mainTable;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QPushButton *addWordButton;

    void UpdateTable();
};
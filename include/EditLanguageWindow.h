#pragma once

#include "stdafx.h"
#include "Language.h"

class LanguageFamily;

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
    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<std::string> Place_;
    std::optional<int> Period_;
    std::optional<Language> Language_;

    QTableWidget *MainTable_;
    QLineEdit *SearchLineEdit_;
    QPushButton *SearchButton_;
    QPushButton *AddWordButton_;

    void UpdateTable();
};
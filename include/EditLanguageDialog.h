#pragma once

#include "stdafx.h"
#include "Language.h"
#include "DialogLayout.h"

class LanguageFamily;

class EditLanguageDialog : public QDialog
{
    Q_OBJECT

public:
    EditLanguageDialog(QWidget *parent = nullptr);
    ~EditLanguageDialog() = default;
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const int place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);

private:
    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<int> Place_;
    std::optional<int> Period_;
    std::optional<Language> Language_;

    DialogLayout Layout_;

    void UpdateTable();
};
#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"

class EditPeriodDialog : public QDialog
{
    Q_OBJECT

public:
    EditPeriodDialog(QWidget *parent = nullptr);
    ~EditPeriodDialog() = default;
    void SetLanguages(LanguageFamily *languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();
    void AddAbove();
    void AddBelow();
    void Remove();

private:
    LanguageFamily *Languages_;
    std::optional<std::string> Place_;
    std::optional<int> Period_;

    QPushButton *AddUpButton_;
    QPushButton *AddDownButton_;
    QPushButton *RemoveButton_;
};
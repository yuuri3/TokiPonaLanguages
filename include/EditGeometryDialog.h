#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"

class EditGeometryDialog : public QDialog
{
    Q_OBJECT

public:
    EditGeometryDialog(QWidget *parent = nullptr);
    ~EditGeometryDialog() = default;
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();

private:
    std::shared_ptr<LanguageFamily> Languages;
    std::optional<std::string> Place;
    std::optional<int> Period;

    QTableWidget *mainTable;

    void UpdateTable();
};
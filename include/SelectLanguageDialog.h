#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"
#include "Language.h"
#include "DialogLayout.h"
#include "TableData.h"

class SelectLanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectLanguageDialog(QWidget *parent = nullptr);
    ~SelectLanguageDialog() override = default;

    void Set(const TableData &languageNames, int *place, int *period);

public slots:
    void Unimplemented();
    void SelectLanguage(int row, int column);
    void OKButtonPushed();

private:
    DialogLayout LayoutData_;

    std::optional<TableData> LanguageNames_;
    int *Place_;
    int *Period_;
    int SelectedPlace_;
    int SelectedPeriod_;
};
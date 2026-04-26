#pragma once

#include "stdafx.h"
#include "DialogLayout.h"
#include "PeriodDifference.h"
#include "TableData.h"

class LanguageFamily;

class EditPeriodDialog : public QDialog
{
    Q_OBJECT

public:
    EditPeriodDialog(QWidget *parent = nullptr);
    ~EditPeriodDialog() = default;
    void Set(std::shared_ptr<LanguageFamily> languages);
    void accept() override;

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();

private:
    std::shared_ptr<LanguageFamily> Languages_;

    DialogLayout Layout_;

    TableData CurrentLanguages_;
    std::vector<PeriodDifference> PeriodDifferences_;

    void UpdateList();
    void ApplyDifference(const PeriodDifference &difference);
};
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
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const int place);
    void SetPeriod(const int period);
    void accept() override;

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();

private:
    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<int> Place_;
    std::optional<int> Period_;

    DialogLayout Layout_;

    TableData CurrentLanguages_;
    std::vector<PeriodDifference> PeriodDifferences_;

    void UpdateList();
    void ApplyDifference(const PeriodDifference &difference);
};
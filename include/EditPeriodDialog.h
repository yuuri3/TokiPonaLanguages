#pragma once

#include "stdafx.h"

class EditPeriodDialog : public QDialog
{
    Q_OBJECT

public:
    EditPeriodDialog(QWidget *parent = nullptr);
    ~EditPeriodDialog() = default;
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();

private:
    std::optional<std::string> Place_;
    std::optional<int> Period_;

    QPushButton *AddUpButton_;
    QPushButton *AddDownButton_;
    QPushButton *RemoveButton_;
};
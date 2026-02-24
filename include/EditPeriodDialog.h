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
    std::optional<std::string> Place;
    std::optional<int> Period;

    QPushButton *addUpButton;
    QPushButton *addDownButton;
    QPushButton *removeButton;
};
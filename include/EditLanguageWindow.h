#pragma once

#include "stdafx.h"
#include "LanguageFamilySimulator.h"

class EditLanguageWindow : public QDialog
{
    Q_OBJECT

public:
    EditLanguageWindow(QWidget *parent = nullptr);
    ~EditLanguageWindow() = default;

private slots:

private:
    QPushButton *OKButton;
};
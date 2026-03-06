#pragma once

#include "stdafx.h"

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() = default;

private slots:

private:
    QPushButton *CloseButton;

    const int BUTTON_WIDTH = 50;
};
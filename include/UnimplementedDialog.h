#pragma once

#include "stdafx.h"

class UnimplementedDialog : public QDialog
{
    Q_OBJECT

public:
    UnimplementedDialog(QWidget *parent = nullptr);
    ~UnimplementedDialog() = default;

private slots:
    void OkButtonClicked();

private:
    QPushButton *OKButton_;
};
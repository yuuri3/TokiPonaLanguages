#pragma once

#include "stdafx.h"
#include "Word.h"

class EditWordDialog : public QDialog
{
    Q_OBJECT

public:
    EditWordDialog(QWidget *parent = nullptr);
    ~EditWordDialog() = default;

private slots:
    void Unimplemented();

private:
};
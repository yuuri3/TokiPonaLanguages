#pragma once

#include "stdafx.h"
#include "DialogLayout.h"

class EditPhonologicalChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPhonologicalChangeDialog(QWidget *parent = nullptr);
    ~EditPhonologicalChangeDialog() override = default;

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void ShowHelp();

private:
    DialogLayout LayoutData_;
};
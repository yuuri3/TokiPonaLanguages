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
    // 未実装呼び出し用スロット
    void Unimplemented();

    // 音韻変化リストの右クリックメニュー表示用スロット
    void ShowContextMenu(const QPoint &pos);

private:
    DialogLayout LayoutData_;
};
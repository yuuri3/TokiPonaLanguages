#pragma once

#include "stdafx.h"

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
    // ヘルプ
    QPushButton *helpButton_;

    // 言語名
    QPushButton *selectLanguageButton_;
    QLineEdit *languageNameEdit_;

    // 音韻変化
    QPushButton *addRuleButton_;
    QListWidget *rulesListWidget_;

    // 音節構造
    QLineEdit *syllableStructureEdit_;

    // 同音語許容ボタン (チェックボックスとして実装)
    QCheckBox *allowHomophonesCheckBox_;

    // 保存ボタン
    QPushButton *okButton_;
    QPushButton *cancelButton_;
};
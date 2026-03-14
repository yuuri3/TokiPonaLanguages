#include "EditPhonologicalChangeDialog.h"
#include "DialogLayout.h"

namespace
{
    constexpr int NAME_ID = 0;
    constexpr int PHONOLOGICAL_CHANGE_ID = 1;
    constexpr int SYLLABLE_ID = 2;
    constexpr int MINIMAL_PAIR_ID = 3;
}

EditPhonologicalChangeDialog::EditPhonologicalChangeDialog(QWidget *parent)
    : QDialog(parent)
{
    // ==========================================
    // 1. レイアウトデータの構築
    // ==========================================
    LayoutData_ = DialogLayout::Create("音韻変化編集", true, true, true);

    // ID 0: 言語名
    LayoutData_.SetTitle(NAME_ID, "言語名");
    LayoutData_.SetDataType(NAME_ID, DialogDataType::String);
    LayoutData_.SetIsEditable(NAME_ID, false);
    LayoutData_.SetHasEditButton(NAME_ID, true);

    // ID 1: 音韻変化
    LayoutData_.SetTitle(PHONOLOGICAL_CHANGE_ID, "音韻変化");
    LayoutData_.SetDataType(PHONOLOGICAL_CHANGE_ID, DialogDataType::StringArray);
    LayoutData_.SetIsEditable(PHONOLOGICAL_CHANGE_ID, true);
    LayoutData_.SetHasAddButton(PHONOLOGICAL_CHANGE_ID, true);
    LayoutData_.SetHasContextMenu(PHONOLOGICAL_CHANGE_ID, true);

    // ID 2: 音節構造
    LayoutData_.SetTitle(SYLLABLE_ID, "音節構造");
    LayoutData_.SetDataType(SYLLABLE_ID, DialogDataType::String);
    LayoutData_.SetIsEditable(SYLLABLE_ID, true);

    // ID 3: 同音語を許容する
    LayoutData_.SetTitle(MINIMAL_PAIR_ID, "同音語を許容する");
    LayoutData_.SetDataType(MINIMAL_PAIR_ID, DialogDataType::Boolean);
    LayoutData_.SetIsEditable(MINIMAL_PAIR_ID, true);

    // ==========================================
    // 2. UIの自動生成と適用
    // ==========================================
    LayoutData_.GenerateLayout(this);
    const auto &ui = LayoutData_.GetUI();
    setLayout(ui.MainLayout);

    // ==========================================
    // 3. シグナルとスロットの接続・初期設定
    // ==========================================
    if (ui.HelpButton)
        connect(ui.HelpButton, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    if (ui.OkButton)
        connect(ui.OkButton, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented); // 実装時は accept などに変更
    if (ui.CancelButton)
        connect(ui.CancelButton, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::reject);

    if (ui.EditButtons.at(NAME_ID))
        connect(ui.EditButtons.at(NAME_ID), &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    if (ui.AddButtons.at(PHONOLOGICAL_CHANGE_ID))
        connect(ui.AddButtons.at(PHONOLOGICAL_CHANGE_ID), &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    auto allowHomophonesCheckBox = qobject_cast<QCheckBox *>(ui.Inputs.at(MINIMAL_PAIR_ID));
    if (allowHomophonesCheckBox)
        connect(allowHomophonesCheckBox, &QCheckBox::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);

    auto rulesListWidget = qobject_cast<QListWidget *>(ui.Inputs.at(PHONOLOGICAL_CHANGE_ID));
    if (rulesListWidget)
    {
        // UI確認用のダミーアイテム追加
        auto *dummyItem = new QListWidgetItem(tr("a > b / _c"));
        dummyItem->setFlags(dummyItem->flags() | Qt::ItemIsEditable);
        rulesListWidget->addItem(dummyItem);

        // 右クリックメニューの接続
        connect(rulesListWidget, &QListWidget::customContextMenuRequested, this, &EditPhonologicalChangeDialog::ShowContextMenu);
    }
}

void EditPhonologicalChangeDialog::Unimplemented()
{
    // 未実装であることをユーザーに通知
    QMessageBox::information(this, tr("未実装"), tr("この機能は現在未実装です。"));
}

void EditPhonologicalChangeDialog::ShowContextMenu(const QPoint &pos)
{
    // 右クリックメニューの構築
    QMenu menu(this);

    QAction *moveUpAction = menu.addAction(tr("上へ移動"));
    QAction *moveDownAction = menu.addAction(tr("下へ移動"));
    menu.addSeparator();
    QAction *deleteAction = menu.addAction(tr("削除"));

    // 各アクションを未実装スロットに接続
    connect(moveUpAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);
    connect(moveDownAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);
    connect(deleteAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);

    // リストウィジェット上のグローバル座標にメニューを表示
    auto rulesListWidget = qobject_cast<QListWidget *>(LayoutData_.GetUI().Inputs.at(PHONOLOGICAL_CHANGE_ID));
    menu.exec(rulesListWidget->mapToGlobal(pos));
}
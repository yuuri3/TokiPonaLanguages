#include "EditPhonologicalChangeDialog.h"
#include "DialogLayout.h"
#include "HelpDialog.h"
#include "SelectLanguageDialog.h"

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
    LayoutData_.SetButton(NAME_ID, "言語選択");

    // ID 1: 音韻変化
    LayoutData_.SetTitle(PHONOLOGICAL_CHANGE_ID, "音韻変化");
    LayoutData_.SetDataType(PHONOLOGICAL_CHANGE_ID, DialogDataType::StringArray);
    LayoutData_.SetIsEditable(PHONOLOGICAL_CHANGE_ID, true);
    LayoutData_.SetButton(PHONOLOGICAL_CHANGE_ID, "追加");
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

    // ==========================================
    // 3. シグナルとスロットの接続・初期設定
    // ==========================================
    LayoutData_.ConnectHelpButtonClicked(this, &EditPhonologicalChangeDialog::ShowHelp);
    LayoutData_.ConnectOKButtonClicked(this, &EditPhonologicalChangeDialog::Unimplemented); // 実装時は accept などに変更
    LayoutData_.ConnectCancelButtonClicked(this, reject);
    LayoutData_.ConnectButtonClicked(NAME_ID, this, &EditPhonologicalChangeDialog::SelectLanguageName);
    LayoutData_.ConnectButtonClicked(PHONOLOGICAL_CHANGE_ID, this, &EditPhonologicalChangeDialog::AddPhonologicalChange);
    LayoutData_.ConnectClicked(MINIMAL_PAIR_ID, this, &EditPhonologicalChangeDialog::Unimplemented);
}

/**
 * @brief 語族をセット
 *
 * @param languages 語族
 */
void EditPhonologicalChangeDialog::SetLanguages(const std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
}

/**
 * @brief 言語名をセット
 *
 * @param languageNames 言語名
 */
void EditPhonologicalChangeDialog::SetLanguageNames(const std::vector<std::vector<std::string>> languageNames)
{
    LanguageNames_ = languageNames;
}

void EditPhonologicalChangeDialog::Unimplemented()
{
    // 未実装であることをユーザーに通知
    QMessageBox::information(this, tr("未実装"), tr("この機能は現在未実装です。"));
}

void EditPhonologicalChangeDialog::ShowContextMenu(const QPoint &pos)
{
    // 1. シグナルの送信元（右クリックされたウィジェット）を取得する
    auto *senderWidget = qobject_cast<QWidget *>(sender());
    if (!senderWidget)
        return;

    // 右クリックメニューの構築
    QMenu menu(this);

    QAction *moveUpAction = menu.addAction(tr("上へ移動"));
    QAction *moveDownAction = menu.addAction(tr("下へ移動"));
    menu.addSeparator();
    QAction *deleteAction = menu.addAction(tr("削除"));

    // 各アクションをスロットに接続
    connect(moveUpAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::MoveRuleUp);
    connect(moveDownAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::MoveRuleDown);
    connect(deleteAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::DeleteRule);

    // 2. 送信元ウィジェットの座標を使って、ローカル座標をグローバル座標に変換しメニューを表示
    menu.exec(senderWidget->mapToGlobal(pos));
}

void EditPhonologicalChangeDialog::ShowHelp()
{
    HelpDialogContent contents;

    contents.AddHeader("音韻変化編集");
    contents.AddContent("言語名", "音韻変化させる言語です。「選択」ボタンから選択できます。");
    contents.AddContent("音韻変化", "ここに音韻変化を記述します。音韻変化は上の行から適用されていきます");
    contents.AddContent("音節構造", "音節構造を制限したい場合、ここに音節構造を記述してください。");
    contents.AddContent("同音語を許容する", "これをチェックした場合、同音語が生まれるかどうかに関わらず音韻変化を実行します。");

    HelpDialog subWindow(this);
    subWindow.SetContents(contents);
    subWindow.exec();
}

/**
 * @brief 言語名選択
 *
 */
void EditPhonologicalChangeDialog::SelectLanguageName()
{
    if (!LanguageNames_)
    {
        return;
    }
    auto subWindow = SelectLanguageDialog(this);
    int place = -1;
    int period = -1;
    subWindow.Set(*LanguageNames_, &place, &period);
    subWindow.exec();
    if (place < 0 || period < 0)
    {
        return;
    }

    LayoutData_.SetText(NAME_ID, LanguageNames_->at(period).at(place));
}

void EditPhonologicalChangeDialog::AddPhonologicalChange()
{
    LayoutData_.AddLine(PHONOLOGICAL_CHANGE_ID, {""}, {});
}

/**
 * @brief 選択された音韻変化規則を1つ上へ移動する
 */
void EditPhonologicalChangeDialog::MoveRuleUp()
{
    // DialogLayout に「現在の選択行」を尋ねるだけ
    int currentRow = LayoutData_.GetCurrentRow(PHONOLOGICAL_CHANGE_ID);

    // 何かしら行が選択されていれば、移動処理を実行
    if (currentRow >= 0)
    {
        LayoutData_.MoveUp(PHONOLOGICAL_CHANGE_ID, currentRow);
    }
}

/**
 * @brief 選択された音韻変化規則を1つ下へ移動する
 */
void EditPhonologicalChangeDialog::MoveRuleDown()
{
    // DialogLayout に「現在の選択行」を尋ねるだけ
    int currentRow = LayoutData_.GetCurrentRow(PHONOLOGICAL_CHANGE_ID);

    // 何かしら行が選択されていれば、移動処理を実行
    if (currentRow >= 0)
    {
        LayoutData_.MoveDown(PHONOLOGICAL_CHANGE_ID, currentRow);
    }
}

/**
 * @brief 選択された音韻変化規則を削除する
 */
void EditPhonologicalChangeDialog::DeleteRule()
{
    // DialogLayout に「現在の選択行」を尋ねるだけ
    int currentRow = LayoutData_.GetCurrentRow(PHONOLOGICAL_CHANGE_ID);

    // 何かしら行が選択されていれば、移動処理を実行
    if (currentRow >= 0)
    {
        LayoutData_.DeleteLine(PHONOLOGICAL_CHANGE_ID, currentRow);
    }
}
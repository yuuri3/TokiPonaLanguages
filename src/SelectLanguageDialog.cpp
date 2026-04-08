#include "SelectLanguageDialog.h"
#include "Utility.h"

namespace
{
    constexpr int TABLE_ID = 0;
    constexpr int SELECTED_LANGUAGE_ID = 1;
}

SelectLanguageDialog::SelectLanguageDialog(QWidget *parent)
    : QDialog(parent)
{
    LayoutData_ = DialogLayout::Create("言語選択", false, true, true);

    // メインテーブル
    LayoutData_.SetTitle(TABLE_ID, "");
    LayoutData_.SetDataType(TABLE_ID, DialogDataType::Table);
    LayoutData_.SetIsEditable(TABLE_ID, false);

    LayoutData_.SetTitle(SELECTED_LANGUAGE_ID, "選択中の言語");
    LayoutData_.SetDataType(SELECTED_LANGUAGE_ID, DialogDataType::String);
    LayoutData_.SetIsEditable(SELECTED_LANGUAGE_ID, false);

    LayoutData_.GenerateLayout(this);

    // ダイアログの要素とイベントの関連
    LayoutData_.ConnectButtonClicked(OK_BUTTON_ID, this, &SelectLanguageDialog::OKButtonPushed);
    LayoutData_.ConnectButtonClicked(CANCEL_BUTTON_ID, this, reject);
    LayoutData_.ConnectClicked(TABLE_ID, this, &SelectLanguageDialog::SelectLanguage);

    LayoutData_.ActivateButton(OK_BUTTON_ID, false);
}

/**
 * @brief セッタ
 *
 * @param languageNames 言語名のデータ
 * @param place 地域
 * @param period 時代
 */
void SelectLanguageDialog::Set(const TableData &languageNames, int *place, int *period)
{
    LanguageNames_ = languageNames;
    LayoutData_.SetData(TABLE_ID, languageNames);
    Place_ = place;
    Period_ = period;
}

/**
 * @brief 未実装項目選択時
 *
 */
void SelectLanguageDialog::Unimplemented()
{
    // 未実装であることをユーザーに通知
    QMessageBox::information(this, tr("未実装"), tr("この機能は現在未実装です。"));
}

/**
 * @brief 言語選択時の処理
 *
 * @param row 行インデックス
 * @param column 列インデックス
 */
void SelectLanguageDialog::SelectLanguage(int row, int column)
{
    // 1行目（インデックス0）と2行目（インデックス1）は選択対象外とする
    if (row < 2 || column < 1)
    {
        return;
    }

    SelectedPlace_ = column - 1;
    SelectedPeriod_ = row - 1;
    LayoutData_.SetText(SELECTED_LANGUAGE_ID, LanguageNames_->Body[SelectedPeriod_][SelectedPlace_]);
    LayoutData_.ActivateButton(OK_BUTTON_ID, true);
}

/**
 * @brief OKボタン押下イベント
 *
 */
void SelectLanguageDialog::OKButtonPushed()
{
    *Place_ = SelectedPlace_;
    *Period_ = SelectedPeriod_;
    accept();
}
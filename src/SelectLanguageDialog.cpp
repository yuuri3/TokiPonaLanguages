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
    auto &ui = LayoutData_.GetUI();
    if (ui.OkButton)
        connect(ui.OkButton, &QPushButton::clicked, this, &SelectLanguageDialog::OKButtonPushed); // 実装時は accept などに変更
    if (ui.CancelButton)
        connect(ui.CancelButton, &QPushButton::clicked, this, &SelectLanguageDialog::reject);
    // メインテーブル
    auto mainTable = qobject_cast<QTableWidget *>(ui.Inputs.at(TABLE_ID));
    connect(mainTable, &QTableWidget::cellClicked, this, &SelectLanguageDialog::SelectLanguage);
}

/**
 * @brief セッタ
 *
 * @param languageNames 言語名のデータ
 * @param place 地域
 * @param period 時代
 */
void SelectLanguageDialog::Set(const std::vector<std::vector<std::string>> languageNames, int *place, int *period)
{
    LanguageNames_ = languageNames;
    auto mainTable = qobject_cast<QTableWidget *>(LayoutData_.GetUI().Inputs.at(TABLE_ID));
    DisplayTable(mainTable, *LanguageNames_);
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
 * @param pos
 */
void SelectLanguageDialog::SelectLanguage(int row, int column)
{
    SelectedPlace_ = column;
    SelectedPeriod_ = row;
    qobject_cast<QLineEdit *>(LayoutData_.GetUI().Inputs.at(SELECTED_LANGUAGE_ID))->setText(QString::fromStdString((*LanguageNames_)[row][column]));
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
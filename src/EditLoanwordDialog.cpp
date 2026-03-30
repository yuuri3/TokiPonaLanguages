#include "EditLoanwordDialog.h"
#include "HelpDialog.h"
#include "SelectLanguageDialog.h"

namespace
{
    constexpr int LANGUAGE_ID = 0;
    constexpr int REFERENCE_LANGUAGE_ID = 1;
    constexpr int WORD_ID = 2;
    constexpr int LOANWORD_ID = 3;
}

EditLoanwordDialog::EditLoanwordDialog(QWidget *parent)
    : QDialog(parent)
{
    // ==========================================
    // 1. レイアウトデータの構築
    // ==========================================
    // "保存"(OK), "キャンセル", "ヘルプ" ボタンを有効化
    layoutData_ = DialogLayout::Create("借用編集", true, true, true);

    // ID 0: 言語
    layoutData_.SetTitle(LANGUAGE_ID, "言語");
    layoutData_.SetDataType(LANGUAGE_ID, DialogDataType::String);
    layoutData_.SetIsEditable(LANGUAGE_ID, false);
    layoutData_.SetButton(LANGUAGE_ID, "選択");

    // ID 1: 参照言語
    layoutData_.SetTitle(REFERENCE_LANGUAGE_ID, "参照言語");
    layoutData_.SetDataType(REFERENCE_LANGUAGE_ID, DialogDataType::String);
    layoutData_.SetIsEditable(REFERENCE_LANGUAGE_ID, false);
    layoutData_.SetButton(REFERENCE_LANGUAGE_ID, "選択");

    // ID 2: 単語
    layoutData_.SetTitle(WORD_ID, "単語");
    layoutData_.SetDataType(WORD_ID, DialogDataType::String);
    layoutData_.SetIsEditable(WORD_ID, false);
    layoutData_.SetButton(WORD_ID, "選択");

    // ID 3: 借用結果（表示領域として StringArray を仮定）
    layoutData_.SetTitle(LOANWORD_ID, "借用履歴");
    layoutData_.SetDataType(LOANWORD_ID, DialogDataType::StringArray);
    layoutData_.SetIsEditable(LOANWORD_ID, true);
    layoutData_.SetButton(LOANWORD_ID, "借用");

    // ==========================================
    // 2. UIの自動生成と適用
    // ==========================================
    layoutData_.GenerateLayout(this);

    // ==========================================
    // 3. シグナルとスロットの接続・初期設定
    // ==========================================
    layoutData_.ConnectButtonClicked(HELP_BUTTON_ID, this, &EditLoanwordDialog::ShowHelp);
    layoutData_.ConnectButtonClicked(OK_BUTTON_ID, this, &EditLoanwordDialog::SaveButtonClicked);
    layoutData_.ConnectButtonClicked(CANCEL_BUTTON_ID, this, reject);

    layoutData_.ConnectButtonClicked(LANGUAGE_ID, this, &EditLoanwordDialog::SelectLanguage);
    layoutData_.ConnectButtonClicked(REFERENCE_LANGUAGE_ID, this, &EditLoanwordDialog::SelectReferenceLanguage);
    layoutData_.ConnectButtonClicked(WORD_ID, this, &EditLoanwordDialog::SelectWord);
    layoutData_.ConnectButtonClicked(LOANWORD_ID, this, &EditLoanwordDialog::BorrowWord);
}

void EditLoanwordDialog::SetLanguages(const std::shared_ptr<LanguageFamily> languages)
{
    languages_ = languages;
}

void EditLoanwordDialog::SetLanguageNames(const std::vector<std::vector<std::string>> languageNames)
{
    languageNames_ = languageNames;
}

void EditLoanwordDialog::ShowHelp()
{
    // TODO: ヘルプ表示の実装
}

void EditLoanwordDialog::SaveButtonClicked()
{
    // TODO: 保存処理の実装
    accept();
}

void EditLoanwordDialog::SelectLanguage()
{
    // TODO: 言語選択の実装
}

void EditLoanwordDialog::SelectReferenceLanguage()
{
    // TODO: 参照言語選択の実装
}

void EditLoanwordDialog::SelectWord()
{
    // TODO: 単語選択の実装
}

void EditLoanwordDialog::BorrowWord()
{
    // TODO: 借用処理の実装
}
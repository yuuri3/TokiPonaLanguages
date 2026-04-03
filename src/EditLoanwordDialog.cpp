#include "stdafx.h"
#include "EditLoanwordDialog.h"
#include "HelpDialog.h"
#include "SelectLanguageDialog.h"
#include "SelectWordDialog.h"

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
    layoutData_.SetTitle(LANGUAGE_ID, "借用先言語");
    layoutData_.SetDataType(LANGUAGE_ID, DialogDataType::String);
    layoutData_.SetIsEditable(LANGUAGE_ID, false);
    layoutData_.SetButton(LANGUAGE_ID, "選択");

    // ID 1: 参照言語
    layoutData_.SetTitle(REFERENCE_LANGUAGE_ID, "借用元言語");
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
    layoutData_.SetIsEditable(LOANWORD_ID, false);
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

    SelectedWordID_ = -1;
}

void EditLoanwordDialog::SetLanguages(const std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
}

void EditLoanwordDialog::SetLanguageNames(const std::vector<std::vector<std::string>> languageNames)
{
    LanguageNames_ = languageNames;
}

/**
 * @brief ヘルプ表示
 *
 */
void EditLoanwordDialog::ShowHelp()
{
    HelpDialogContent contents;

    contents.AddHeader("借用編集");
    contents.AddContent("言語", "借用先の言語（単語を受け入れる側の言語）です。「選択」ボタンから選択できます。");
    contents.AddContent("参照言語", "借用元の言語（単語を提供する側の言語）です。「選択」ボタンから選択できます。");
    contents.AddContent("単語", "借用する単語です。参照言語を選択後、「選択」ボタンから選ぶことができます。");
    contents.AddContent("借用履歴", "これまでに借用した単語の履歴が表示されます。「借用」ボタンを押すことで、指定した単語が借用されてここに追加されます。");

    HelpDialog subWindow(this);
    subWindow.SetContents(contents);
    subWindow.exec();
}

void EditLoanwordDialog::SaveButtonClicked()
{
    if (Languages_ && TargetLanguage_ && ReferenceLanguage_)
    {
        auto currentTargetWordIDs = Languages_->GetLoanwordIDs(TargetPlace_, ReferencePlace_, TargetPeriod_) | std::ranges::views::keys | std::ranges::to<std::vector>();
        Languages_->SetLoanwords(TargetPlace_, ReferencePlace_, TargetPeriod_, LoanwordIDs_);
    }
    accept();
}

/**
 * @brief 言語選択
 *
 */
void EditLoanwordDialog::SelectLanguage()
{
    if (!LanguageNames_ || !Languages_)
    {
        return;
    }

    SelectLanguageDialog subWindow(this);
    int place = -1;
    int period = -1;

    // 言語選択ダイアログを表示
    subWindow.Set(*LanguageNames_, &place, &period);
    subWindow.exec();

    if (place < 0 || period < 0)
    {
        return;
    }

    // 選択された言語名をUIに反映
    layoutData_.SetText(LANGUAGE_ID, LanguageNames_->at(period).at(place));

    // 内部状態の更新（EditPhonologicalChangeDialog の実装を参考）
    TargetPlace_ = LanguageNames_->at(0).at(place);
    TargetPeriod_ = period - 1;
    TargetLanguage_ = Languages_->CalculateLanguage(TargetPlace_, TargetPeriod_);

    if (TargetLanguage_)
    {
        TargetWordID_ = TargetLanguage_->GetNewWordID();
    }
    if (TargetLanguage_ && ReferenceLanguage_)
    {
        LoanwordIDs_ = Languages_->GetLoanwordIDs(TargetPlace_, ReferencePlace_, TargetPeriod_);
    }
    DisplayLoanword();
}

/**
 * @brief 参照言語選択
 *
 */
void EditLoanwordDialog::SelectReferenceLanguage()
{
    if (!LanguageNames_ || !Languages_)
    {
        return;
    }

    SelectLanguageDialog subWindow(this);
    int place = -1;
    int period = -1;

    // 言語選択ダイアログを表示
    subWindow.Set(*LanguageNames_, &place, &period);
    subWindow.exec();

    if (place < 0 || period < 0)
    {
        return;
    }

    // 選択された言語名をUIの参照言語テキストボックスに反映
    layoutData_.SetText(REFERENCE_LANGUAGE_ID, LanguageNames_->at(period).at(place));

    // 後続の借用処理のため、内部状態の更新を推奨
    ReferencePlace_ = LanguageNames_->at(0).at(place);
    ReferencePeriod_ = period - 1;
    ReferenceLanguage_ = Languages_->CalculateLanguage(ReferencePlace_, ReferencePeriod_);

    if (TargetLanguage_ && ReferenceLanguage_)
    {
        LoanwordIDs_ = Languages_->GetLoanwordIDs(TargetPlace_, ReferencePlace_, TargetPeriod_);
    }
    DisplayLoanword();
}

void EditLoanwordDialog::SelectWord()
{
    if (!ReferenceLanguage_)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "「参照元言語」を選択してください");
    }
    // 参照言語が未選択の場合や、データが存在しない場合は何もしない
    if (!Languages_ || !LanguageNames_ || ReferencePeriod_ < 0)
    {
        return;
    }

    // 参照言語の計算

    // 単語選択ダイアログの生成と設定
    SelectWordDialog subWindow(this);
    subWindow.SetLanguageFamily(Languages_);

    SelectedWordID_ = -1;
    subWindow.SetLanguage(std::make_shared<Language>(ReferenceLanguage_.value()), &SelectedWordID_);
    subWindow.exec();

    // 単語が選択された場合の処理
    if (SelectedWordID_ >= 0)
    {
        std::string wordString = Languages_->GetPhonemeTable().ConvertToString(ReferenceLanguage_->GetWord(SelectedWordID_)->GetForm());

        // UI に選択した単語の文字列を反映
        layoutData_.SetText(WORD_ID, wordString);

        LoanwordIDs_.emplace_back(SelectedWordID_, TargetWordID_);
    }

    DisplayLoanword();
}

void EditLoanwordDialog::BorrowWord()
{
    if (!TargetLanguage_)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "「借用先言語」を選択してください");
    }
    else if (!ReferenceLanguage_)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "「借用元言語」を選択してください");
    }
    else if (SelectedWordID_ < 0)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "「単語」を選択してください");
    }

    const auto targetWordID = TargetLanguage_->GetNewWordID();
    const auto referenceWordID = SelectedWordID_;
    LanguageDifference dif = LanguageDifference::CreateLoanword(ReferencePlace_, TargetPlace_, TargetPeriod_, referenceWordID, targetWordID);
    Languages_->AddDifference(dif);
    DisplayLoanword();
}

/**
 * @brief 借用語の一覧を表示
 *
 */
void EditLoanwordDialog::DisplayLoanword()
{
    if (!Languages_ || !TargetLanguage_ || !ReferenceLanguage_)
    {
        return;
    }

    std::vector<std::string> loanwords;
    for (const auto [referenceWordID, _] : LoanwordIDs_)
    {
        loanwords.emplace_back(Languages_->GetPhonemeTable().ConvertToString(ReferenceLanguage_->GetWord(referenceWordID)->GetForm()));
    }
    layoutData_.Clear(LOANWORD_ID);
    layoutData_.AddLine(LOANWORD_ID, loanwords, {});
}
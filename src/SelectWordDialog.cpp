#include "SelectWordDialog.h"
#include "Utility.h"
#include "HelpDialog.h"

namespace
{
    constexpr int tableId = 0;
    constexpr int searchWordId = 1;
    constexpr int searchButtonId = 2;
}

SelectWordDialog::SelectWordDialog(QWidget *parent)
    : QDialog(parent), SelectedWordId_(nullptr), CurrentSelectedWordId_(-1)
{
    // ヘルプ, OK, キャンセルボタンを有効化してレイアウト生成
    LayoutData_ = DialogLayout::Create("単語選択", true, true, true);

    // 単語検索
    LayoutData_.SetTitle(searchWordId, "単語検索");
    LayoutData_.SetDataType(searchWordId, DialogDataType::String);

    // 検索ボタン
    LayoutData_.SetTitle(searchButtonId, "");
    LayoutData_.SetDataType(searchButtonId, DialogDataType::NoData);
    LayoutData_.SetButton(searchButtonId, "検索");

    // 単語一覧
    LayoutData_.SetTitle(tableId, "単語");
    LayoutData_.SetDataType(tableId, DialogDataType::Table);
    LayoutData_.SetIsEditable(tableId, false);

    LayoutData_.GenerateLayout(this);

    // イベントの関連付け
    LayoutData_.ConnectButtonClicked(HELP_BUTTON_ID, this, &SelectWordDialog::ShowHelp);
    LayoutData_.ConnectButtonClicked(OK_BUTTON_ID, this, &SelectWordDialog::OKButtonPushed);
    LayoutData_.ConnectButtonClicked(CANCEL_BUTTON_ID, this, &SelectWordDialog::reject);
    LayoutData_.ConnectButtonClicked(searchButtonId, this, &SelectWordDialog::SearchWord);
    LayoutData_.ConnectClicked(tableId, this, &SelectWordDialog::SelectWord);

    LayoutData_.ActivateButton(OK_BUTTON_ID, false);
}

/**
 * @brief 語族のセッタ
 *
 * @param languageFamily 対象の語族
 */
void SelectWordDialog::SetLanguageFamily(std::shared_ptr<LanguageFamily> languageFamily)
{
    LanguageFamily_ = languageFamily;
}

/**
 * @brief 言語・出力先ポインタのセッタ
 *
 * @param targetLanguage 対象の言語
 * @param outSelectedWordId 選択された単語IDを格納するポインタ
 */
void SelectWordDialog::SetLanguage(std::shared_ptr<Language> targetLanguage, int *outSelectedWordId)
{
    Language_ = targetLanguage;
    SelectedWordId_ = outSelectedWordId;

    if (!Language_)
    {
        return;
    }

    std::vector<std::vector<std::string>> wordData;
    std::vector<std::string> header;

    header.emplace_back("単語");
    header.emplace_back("訳語");
    wordData.emplace_back(header);

    for (int i = 0; i < Language_->CountWord(); i++)
    {
        std::vector<std::string> rowData;
        const auto &[wordId, word] = Language_->GetNthWord(i);

        std::string formString;
        rowData.emplace_back(LanguageFamily_->GetPhonemeTable().ConvertToString(word.GetForm()));

        const auto translations = word.GetAllTranslations();
        rowData.emplace_back(JoinStrs(translations, ","));

        wordData.emplace_back(rowData);
    }

    LayoutData_.SetDataToTable(tableId, wordData);
}

/**
 * @brief ヘルプ表示
 *
 */
void SelectWordDialog::ShowHelp()
{
    HelpDialogContent contents;

    contents.AddHeader("単語選択");
    contents.AddContent("単語検索", "検索したい単語を入力する欄です。（※現在この機能は未実装です）");
    contents.AddContent("検索ボタン", "入力した条件で単語を検索します。（※現在この機能は未実装です）");
    contents.AddContent("単語", "選択可能な単語とその訳語の一覧が表示されます。リストから行をクリックして選択し、OKボタンを押すことで対象の単語を決定できます。");

    HelpDialog subWindow(this);
    subWindow.SetContents(contents);
    subWindow.exec();
}

/**
 * @brief 未実装項目選択時
 *
 */
void SelectWordDialog::Unimplemented()
{
    QMessageBox::information(this, tr("未実装"), tr("この機能は現在未実装です。"));
}

/**
 * @brief 検索ボタン押下時
 *
 */
void SelectWordDialog::SearchWord()
{
    Unimplemented();
}

/**
 * @brief 単語選択時の処理
 *
 * @param row 行インデックス
 * @param column 列インデックス
 */
void SelectWordDialog::SelectWord(int row, int column)
{
    // ヘッダ行（インデックス0）は選択対象外とする
    if (row < 1 || !Language_)
    {
        return;
    }

    if (row - 1 < Language_->CountWord())
    {
        const auto &[wordId, _] = Language_->GetNthWord(row - 1);
        CurrentSelectedWordId_ = wordId;
        LayoutData_.ActivateButton(OK_BUTTON_ID, true);
    }
}

/**
 * @brief OKボタン押下イベント
 *
 */
void SelectWordDialog::OKButtonPushed()
{
    if (SelectedWordId_)
    {
        *SelectedWordId_ = CurrentSelectedWordId_;
    }
    accept();
}
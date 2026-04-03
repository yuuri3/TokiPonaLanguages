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

    // テーブルの初期表示を兼ねて検索処理を呼び出す
    SearchWord();
}

/**
 * @brief ヘルプ表示
 *
 */
void SelectWordDialog::ShowHelp()
{
    HelpDialogContent contents;

    contents.AddHeader("単語選択");
    contents.AddContent("単語検索", "検索したい単語を入力する欄です。");
    contents.AddContent("検索ボタン", "入力した条件で単語（見出し語・訳語）を部分一致検索します。");
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
 * @brief 検索ボタン押下時の処理（見出し語・訳語の部分一致検索）
 *
 */
void SelectWordDialog::SearchWord()
{
    if (!Language_ || !LanguageFamily_)
    {
        return;
    }

    std::string query = "";
    auto lines = LayoutData_.GetLine(searchWordId);
    if (!lines.empty())
    {
        query = lines[0];
    }

    std::vector<std::vector<std::string>> wordData;
    std::vector<std::string> header;

    header.emplace_back("単語");
    header.emplace_back("訳語");
    wordData.emplace_back(header);

    DisplayedWordIds_.clear();

    for (int i = 0; i < Language_->CountWord(); i++)
    {
        const auto &[wordId, word] = Language_->GetNthWord(i);

        std::string formString = LanguageFamily_->GetPhonemeTable().ConvertToString(word.GetForm());
        std::string translationString = JoinStrs(word.GetAllTranslations(), ",");

        // 検索クエリが空、または見出し語・訳語のいずれかに部分一致する場合に追加
        if (query.empty() ||
            formString.find(query) != std::string::npos ||
            translationString.find(query) != std::string::npos)
        {
            std::vector<std::string> rowData;
            rowData.emplace_back(formString);
            rowData.emplace_back(translationString);
            wordData.emplace_back(rowData);

            DisplayedWordIds_.push_back(wordId);
        }
    }

    LayoutData_.SetDataToTable(tableId, wordData);
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

    // 表示されているIDリストから選択された単語のIDを取得
    if (row - 1 < DisplayedWordIds_.size())
    {
        CurrentSelectedWordId_ = DisplayedWordIds_[row - 1];
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
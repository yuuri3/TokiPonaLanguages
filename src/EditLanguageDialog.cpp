#include "EditLanguageDialog.h"
#include "UnimplementedDialog.h"
#include "EditWordDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"
#include "TableData.h"

constexpr int tableId = 0;
constexpr int searchId = 1;
constexpr int addWordId = 2;

EditLanguageDialog::EditLanguageDialog(QWidget *parent)
{
    Layout_ = DialogLayout::Create("個別言語編集", false, false, false);

    // 単語表示
    Layout_.SetTitle(tableId, "");
    Layout_.SetDataType(tableId, DialogDataType::Table);
    Layout_.SetHasContextMenu(tableId, true);

    // 検索バー
    Layout_.SetTitle(searchId, "検索");
    Layout_.SetDataType(searchId, DialogDataType::String);
    Layout_.SetButton(searchId, "検索");

    // 単語追加ボタン
    Layout_.SetTitle(addWordId, "");
    Layout_.SetDataType(addWordId, DialogDataType::NoData);
    Layout_.SetButton(addWordId, "単語追加");

    Layout_.GenerateLayout(this);

    Layout_.ConnectContextMenu(tableId, this, &EditLanguageDialog::ShowContextMenu);
    Layout_.ConnectButtonClicked(searchId, this, &EditLanguageDialog::Unimplemented);
    Layout_.ConnectButtonClicked(addWordId, this, &EditLanguageDialog::Unimplemented);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditLanguageDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditLanguageDialog::SetPlace(const std::string &place)
{
    Place_ = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditLanguageDialog::SetPeriod(const int period)
{
    Period_ = period;
    UpdateTable();
}

/**
 * @brief 表更新
 *
 */
void EditLanguageDialog::UpdateTable()
{
    if (Languages_ && Place_ && Period_)
    {
        auto language = Languages_->CalculateLanguage(*Place_, *Period_);
        if (!language)
        {
            return;
        }

        std::vector<std::string> header = {"単語", "訳語"};
        std::vector<std::vector<std::string>> wordData;
        std::vector<std::string> line;

        for (int i = 0; i < language->CountWord(); i++)
        {
            const auto &[_, word] = language->GetNthWord(i);

            line.emplace_back(Languages_->GetPhonemeTable().ConvertToString(word.GetForm()));

            const auto translations = word.GetAllTranslations();
            line.emplace_back(JoinStrs(translations, ","));

            wordData.emplace_back(line);
            line.clear();
        }

        TableData data = {header, wordData};
        Layout_.SetData(tableId, data);
    }
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditLanguageDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief 単語編集メニュー表示
 *
 */
void EditLanguageDialog::ShowContextMenu(const QPoint &pos)
{
    if (Languages_ && Place_ && Period_)
    {
        const int row = Layout_.GetCurrentRow(tableId);
        if (row <= 0) // 未選択(-1)やヘッダ行(0)なら何もしない
        {
            return;
        }

        auto *widget = qobject_cast<QWidget *>(sender());
        if (!widget)
        {
            return;
        }

        QMenu menu(this);
        QAction *editAction = menu.addAction("編集");

        // メニューを表示し、選ばれたアクションを取得
        QAction *selectedAction = menu.exec(widget->mapToGlobal(pos));

        if (selectedAction == editAction)
        {
            std::optional<Language> language;
            if (Language_)
            {
                language = Language_;
            }
            else
            {
                language = Languages_->CalculateLanguage(*Place_, *Period_);
                Language_ = language;
                if (!language)
                {
                    return;
                }
            }

            if (row - 1 >= language->CountWord())
            {
                return;
            }
            const auto &[wordID, _] = language->GetNthWord(row - 1);

            EditWordDialog subWindow(this);
            subWindow.Set(Languages_, std::make_shared<Language>(Language_.value()), *Place_, *Period_, wordID);
            subWindow.exec();
        }
    }
}
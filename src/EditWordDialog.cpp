#include "EditWordDialog.h"
#include "UnimplementedDialog.h"
#include "Utility.h"
#include "DialogLayout.h"

namespace
{
    constexpr int FORM_ID = 0;
    constexpr int TRANSLATION_ID = 1;
    constexpr int TAG_ID = 2;
    constexpr int CONTENT_ID = 3;
    constexpr int VARIATION_ID = 4;
    constexpr int RELATION_ID = 5;
}

EditWordDialog::EditWordDialog(QWidget *parent)
    : QDialog(parent)
{
    // ==========================================
    // 1. レイアウトデータの構築
    // ==========================================
    // ※元のコードにヘルプ・OK・キャンセルボタンが無いため false に設定しています
    LayoutData_ = DialogLayout::Create("単語編集", false, false, false);

    // ID 0: 見出し語
    LayoutData_.SetTitle(FORM_ID, "見出し語");
    LayoutData_.SetDataType(FORM_ID, DialogDataType::String);
    LayoutData_.SetIsEditable(FORM_ID, true);

    // ID 1: 訳語
    LayoutData_.SetTitle(TRANSLATION_ID, "訳語");
    LayoutData_.SetDataType(TRANSLATION_ID, DialogDataType::StringPairArray);
    LayoutData_.SetButton(TRANSLATION_ID, "追加");

    // ID 2: タグ
    LayoutData_.SetTitle(TAG_ID, "タグ");
    LayoutData_.SetDataType(TAG_ID, DialogDataType::StringPairArray);
    LayoutData_.SetButton(TAG_ID, "追加");

    // ID 3: 自由記述
    LayoutData_.SetTitle(CONTENT_ID, "自由記述");
    LayoutData_.SetDataType(CONTENT_ID, DialogDataType::StringPairArray);
    LayoutData_.SetButton(CONTENT_ID, "追加");

    // ID 4: 変化形
    LayoutData_.SetTitle(VARIATION_ID, "変化形");
    LayoutData_.SetDataType(VARIATION_ID, DialogDataType::StringPairArray);
    LayoutData_.SetButton(VARIATION_ID, "追加");

    // ID 5: 関連語
    LayoutData_.SetTitle(RELATION_ID, "関連語");
    LayoutData_.SetDataType(RELATION_ID, DialogDataType::StringPairArray);
    LayoutData_.SetButton(RELATION_ID, "追加");

    // ==========================================
    // 2. UIの自動生成と適用
    // ==========================================
    LayoutData_.GenerateLayout(this);

    // ==========================================
    // 3. ボタンの幅設定とシグナル・スロットの接続
    // ==========================================
    LayoutData_.ConnectButtonClicked(TRANSLATION_ID, this, &EditWordDialog::AddTranslationButtonPushed);
    LayoutData_.ConnectButtonClicked(TAG_ID, this, &EditWordDialog::AddTagsButtonPushed);
    LayoutData_.ConnectButtonClicked(CONTENT_ID, this, &EditWordDialog::AddContentsButtonPushed);
    LayoutData_.ConnectButtonClicked(VARIATION_ID, this, &EditWordDialog::AddVariationsButtonPushed);
    LayoutData_.ConnectButtonClicked(RELATION_ID, this, &EditWordDialog::AddRelationsButtonPushed);

    constexpr int WINDOW_HEIGHT = 400;
    constexpr int WINDOW_WIDTH = 300;

    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditWordDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief メンバをセット
 *
 * @param languages 語族
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 */
void EditWordDialog::Set(std::shared_ptr<LanguageFamily> languages,
                         std::shared_ptr<Language> language,
                         const std::string &place,
                         const int period,
                         const int wordID)
{
    Languages_ = languages;
    Language_ = language;
    Place_ = place;
    Period_ = period;
    WordID_ = wordID;
    UpdateDialog();
}

/**
 * @brief ダイアログ更新
 *
 */
void EditWordDialog::UpdateDialog()
{
    if (Languages_ && Language_)
    {
        const auto word = Language_->GetWord(WordID_);
        if (!word)
        {
            return;
        }

        // 語形
        const auto form = word->GetForm();
        LayoutData_.SetText(FORM_ID, Languages_->GetPhonemeTable().ConvertToString(form));

        // 訳語
        std::vector<std::vector<std::string>> translations;
        for (const int partID : word->GetPartIDs())
        {
            std::vector<std::string> forms;
            const auto title = word->GetPart(partID);
            for (const int translationID : word->GetTranslationIDs(partID))
            {
                forms.emplace_back(word->GetTranslation(partID, translationID));
            }
            std::string formsStr = JoinStrs(forms, ",");
            translations.push_back({title, formsStr});
        }
        translations.push_back({"", ""});
        DisplayLine(TRANSLATION_ID, translations);

        // タグ
        std::vector<std::vector<std::string>> tags;
        for (const int tagID : word->GetTagIDs())
        {
            const auto tag = word->GetTag(tagID);
            tags.push_back({tag});
        }
        tags.push_back({""});
        DisplayLine(TAG_ID, tags);

        // 自由記述
        std::vector<std::vector<std::string>> contentsData;
        for (const int contentID : word->GetContentIDs())
        {
            const auto title = word->GetContentTitle(contentID);
            const auto content = word->GetContent(contentID);
            contentsData.push_back({title, content});
        }
        contentsData.push_back({"", ""});
        DisplayLine(CONTENT_ID, contentsData);

        // 変化形
        std::vector<std::vector<std::string>> variationsData;
        for (const int variationID : word->GetVariationIDs())
        {
            const auto title = word->GetVariationTitle(variationID);
            const auto variation = word->GetVariation(variationID);
            variationsData.push_back({title, Languages_->GetPhonemeTable().ConvertToString(variation)});
        }
        variationsData.push_back({"", ""});
        DisplayLine(VARIATION_ID, variationsData);

        // 関連語
        std::vector<std::vector<std::string>> relationsData;
        for (const int relationID : word->GetRelationIDs())
        {
            const auto title = word->GetRelationTitle(relationID);
            const int relatedWordID = word->GetRelationWordID(relationID);
            const auto relatedWord = Language_->GetWord(relatedWordID);
            relationsData.push_back({title, Languages_->GetPhonemeTable().ConvertToString(relatedWord->GetForm())});
        }
        relationsData.push_back({"", ""});
        DisplayLine(RELATION_ID, relationsData);
    }
}

/**
 * @brief 行を表示
 *
 * @param widget 表示対象ウィジェット
 * @param values 値のリスト
 * @param widths 各列の幅
 */
void EditWordDialog::DisplayLine(const int id, const std::vector<std::vector<std::string>> &values)
{
    LayoutData_.Clear(id);
    LayoutData_.SetData(id, values);
}

/**
 * @brief 訳語行追加
 *
 */
void EditWordDialog::AddTranslationButtonPushed()
{
    AddLine(TRANSLATION_ID, {"", ""});
}

/**
 * @brief タグ追加ボタンクリック
 *
 */
void EditWordDialog::AddTagsButtonPushed()
{
    AddLine(TAG_ID, {""});
}

/**
 * @brief 自由記述追加ボタンクリック
 *
 */
void EditWordDialog::AddContentsButtonPushed()
{
    AddLine(CONTENT_ID, {"", ""});
}

/**
 * @brief 変化形追加ボタンクリック
 *
 */
void EditWordDialog::AddVariationsButtonPushed()
{
    AddLine(VARIATION_ID, {"", ""});
}

/**
 * @brief 関連語追加ボタンクリック
 *
 */
void EditWordDialog::AddRelationsButtonPushed()
{
    AddLine(RELATION_ID, {"", ""});
}

/**
 * @brief 行追加
 *
 * @param widget 親ウィジェット
 * @param values 入力値（[0]: タイトル, [1]: 内容）
 */
void EditWordDialog::AddLine(const int id, const std::vector<std::string> &values)
{
    LayoutData_.AddLineAndConnectRightClicked(id, values, this, [this, id](const QPoint &pos)
                                              { ClickLine(id, pos); });
}

/**
 * @brief 行クリックイベント
 *
 * @param pos クリック位置
 */
void EditWordDialog::ClickLine(const int id, const QPoint &pos)
{
    // QLineEdit か QTextEdit かを問わず QWidget として取得
    QWidget *senderWidget = qobject_cast<QWidget *>(sender());
    if (!senderWidget)
        return;

    QMenu menu(this);
    QAction *addAction = menu.addAction("追加");
    QAction *removeAction = menu.addAction("削除");

    QAction *selectedAction = menu.exec(senderWidget->mapToGlobal(pos));

    if (selectedAction == addAction)
    {
        const int columnCount = LayoutData_.GetLine(id).at(0).size();

        // ボックスの数に応じた幅設定を維持
        auto widths = (columnCount == 1) ? ONE_WIDTH : TWO_WIDTHS;
        std::vector<std::string> newValues(columnCount, "");
        AddLine(id, newValues);
    }
    else if (selectedAction == removeAction)
    {
        const auto lineIndex = LayoutData_.GetCurrentRow(id);
        LayoutData_.DeleteLine(id, lineIndex);
    }
}
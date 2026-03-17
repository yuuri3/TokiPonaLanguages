#include "EditWordDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamilySimulator.h"
#include "PhonemeConverter.h"
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
    LayoutData_.SetHasAddButton(TRANSLATION_ID, true);

    // ID 2: タグ
    LayoutData_.SetTitle(TAG_ID, "タグ");
    LayoutData_.SetDataType(TAG_ID, DialogDataType::StringPairArray);
    LayoutData_.SetHasAddButton(TAG_ID, true);

    // ID 3: 自由記述
    LayoutData_.SetTitle(CONTENT_ID, "自由記述");
    LayoutData_.SetDataType(CONTENT_ID, DialogDataType::StringPairArray);
    LayoutData_.SetHasAddButton(CONTENT_ID, true);

    // ID 4: 変化形
    LayoutData_.SetTitle(VARIATION_ID, "変化形");
    LayoutData_.SetDataType(VARIATION_ID, DialogDataType::StringPairArray);
    LayoutData_.SetHasAddButton(VARIATION_ID, true);

    // ID 5: 関連語
    LayoutData_.SetTitle(RELATION_ID, "関連語");
    LayoutData_.SetDataType(RELATION_ID, DialogDataType::StringPairArray);
    LayoutData_.SetHasAddButton(RELATION_ID, true);

    // ==========================================
    // 2. UIの自動生成と適用
    // ==========================================
    LayoutData_.GenerateLayout(this);
    const auto &ui = LayoutData_.GetUI();
    setLayout(ui.MainLayout);

    // ==========================================
    // 3. ボタンの幅設定とシグナル・スロットの接続
    // ==========================================
    auto addTranslationButton = ui.AddButtons.at(TRANSLATION_ID);
    if (addTranslationButton)
    {
        connect(addTranslationButton, &QPushButton::clicked, this, &EditWordDialog::AddTranslationButtonPushed);
    }
    auto addTagsButton = ui.AddButtons.at(TAG_ID);
    if (addTagsButton)
    {
        connect(addTagsButton, &QPushButton::clicked, this, &EditWordDialog::AddTagsButtonPushed);
    }
    auto addContentsButton = ui.AddButtons.at(CONTENT_ID);
    if (addContentsButton)
    {
        connect(addContentsButton, &QPushButton::clicked, this, &EditWordDialog::AddContentsButtonPushed);
    }
    auto addVariationsButton = ui.AddButtons.at(VARIATION_ID);
    if (addVariationsButton)
    {
        connect(addVariationsButton, &QPushButton::clicked, this, &EditWordDialog::AddVariationsButtonPushed);
    }
    auto addRelationsButton = ui.AddButtons.at(RELATION_ID);
    if (addRelationsButton)
    {
        connect(addRelationsButton, &QPushButton::clicked, this, &EditWordDialog::AddRelationsButtonPushed);
    }
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
                         const Language &language,
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
    if (Languages_)
    {
        PhonemeConverter converter = PhonemeConverter::Create(Languages_->GetPhonemeTable());

        const auto word = Language_.GetWord(WordID_);
        if (!word)
        {
            return;
        }

        // 語形
        const auto form = word->GetForm();
        auto entry = qobject_cast<QLineEdit *>(LayoutData_.GetUI().Inputs.at(FORM_ID));
        entry->setText(QString::fromStdString(converter.ConvertToString(form)));

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
        DisplayLine(TRANSLATION_ID, translations, TWO_WIDTHS);

        // タグ
        std::vector<std::vector<std::string>> tags;
        for (const int tagID : word->GetTagIDs())
        {
            const auto tag = word->GetTag(tagID);
            tags.push_back({tag});
        }
        tags.push_back({""});
        DisplayLine(TAG_ID, tags, ONE_WIDTH);

        // 自由記述
        std::vector<std::vector<std::string>> contentsData;
        for (const int contentID : word->GetContentIDs())
        {
            const auto title = word->GetContentTitle(contentID);
            const auto content = word->GetContent(contentID);
            contentsData.push_back({title, content});
        }
        contentsData.push_back({"", ""});
        DisplayLine(CONTENT_ID, contentsData, TWO_WIDTHS);

        // 変化形
        std::vector<std::vector<std::string>> variationsData;
        for (const int variationID : word->GetVariationIDs())
        {
            const auto title = word->GetVariationTitle(variationID);
            const auto variation = word->GetVariation(variationID);
            variationsData.push_back({title, converter.ConvertToString(variation)});
        }
        variationsData.push_back({"", ""});
        DisplayLine(VARIATION_ID, variationsData, TWO_WIDTHS);

        // 関連語
        std::vector<std::vector<std::string>> relationsData;
        for (const int relationID : word->GetRelationIDs())
        {
            const auto title = word->GetRelationTitle(relationID);
            const int relatedWordID = word->GetRelationWordID(relationID);
            const auto relatedWord = Language_.GetWord(relatedWordID);
            relationsData.push_back({title, converter.ConvertToString(relatedWord->GetForm())});
        }
        relationsData.push_back({"", ""});
        DisplayLine(RELATION_ID, relationsData, TWO_WIDTHS);
    }
}

/**
 * @brief 行を表示
 *
 * @param widget 表示対象ウィジェット
 * @param values 値のリスト
 * @param widths 各列の幅
 */
void EditWordDialog::DisplayLine(const int id, const std::vector<std::vector<std::string>> &values, const std::vector<int> &widths)
{
    LayoutData_.Clear(id);
    for (const auto value : values)
    {
        AddLine(id, value, widths);
    }
}

/**
 * @brief 訳語行追加
 *
 */
void EditWordDialog::AddTranslationButtonPushed()
{
    AddLine(TRANSLATION_ID, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief タグ追加ボタンクリック
 *
 */
void EditWordDialog::AddTagsButtonPushed()
{
    AddLine(TAG_ID, {""}, ONE_WIDTH);
}

/**
 * @brief 自由記述追加ボタンクリック
 *
 */
void EditWordDialog::AddContentsButtonPushed()
{
    AddLine(CONTENT_ID, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 変化形追加ボタンクリック
 *
 */
void EditWordDialog::AddVariationsButtonPushed()
{
    AddLine(VARIATION_ID, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 関連語追加ボタンクリック
 *
 */
void EditWordDialog::AddRelationsButtonPushed()
{
    AddLine(RELATION_ID, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 行追加
 *
 * @param widget 親ウィジェット
 * @param values 入力値（[0]: タイトル, [1]: 内容）
 * @param widths 幅のリスト
 */
void EditWordDialog::AddLine(const int id, const std::vector<std::string> &values, const std::vector<int> &widths)
{
    auto widgets = LayoutData_.AddLine(id, values, widths);
    for (auto line : widgets)
    {
        if (!line)
        {
            continue;
        }
        connect(qobject_cast<QLineEdit *>(line), &QLineEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
    }
}

/**
 * @brief 行クリックイベント
 *
 * @param pos クリック位置
 */
void EditWordDialog::ClickLine(const QPoint &pos)
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
        QWidget *rowContainer = senderWidget->parentWidget();
        QWidget *targetWidget = rowContainer->parentWidget();
        int targetWidgetID = -1;
        for (const auto [id, widget] : LayoutData_.GetUI().Inputs)
        {
            if (widget == targetWidget)
            {
                targetWidgetID = id;
            }
        }

        // 1つの rowContainer 内にある入力要素（QLineEdit と QTextEdit）の合計数を取得
        int boxCount = rowContainer->findChildren<QLineEdit *>().count() +
                       rowContainer->findChildren<QTextEdit *>().count();

        // ボックスの数に応じた幅設定を維持
        auto widths = (boxCount == 1) ? ONE_WIDTH : TWO_WIDTHS;
        std::vector<std::string> newValues(boxCount, "");
        AddLine(targetWidgetID, newValues, widths);
    }
    else if (selectedAction == removeAction)
    {
        QWidget *rowContainer = senderWidget->parentWidget();
        if (rowContainer)
        {
            DeleteWidget(rowContainer);
        }
    }
}
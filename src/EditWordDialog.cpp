#include "EditWordDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamilySimulator.h"

EditWordDialog::EditWordDialog(QWidget *parent)
{
    setWindowTitle("単語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // * 見出し語
    layout->addWidget(new QLabel("見出し語", this));

    Entry = new QLineEdit(this);
    Entry->setText("テスト単語");
    layout->addWidget(Entry);

    // * 訳語
    QHBoxLayout *translationsTitleLayout = new QHBoxLayout(this);
    translationsTitleLayout->addWidget(new QLabel("訳語", this));
    layout->addLayout(translationsTitleLayout);

    TranslationLayout = new QVBoxLayout(this);
    layout->addLayout(TranslationLayout);

    std::vector<std::pair<std::string, std::string>> translations = {{"", ""}};
    DisplayTranslations(TranslationLayout, translations);

    //   * 訳語追加ボタン
    AddTranslationButton = new QPushButton("訳語追加", this);
    translationsTitleLayout->addWidget(AddTranslationButton);

    // * タグ
    layout->addWidget(new QLabel("タグ", this));

    Tags = new QTableWidget(this);
    layout->addWidget(Tags);

    // * 自由記述
    layout->addWidget(new QLabel("自由記述", this));

    Contents = new QTableWidget(this);
    layout->addWidget(Contents);

    // * 変化形
    layout->addWidget(new QLabel("変化形", this));

    Variations = new QTableWidget(this);
    layout->addWidget(Variations);

    // * 関連語
    layout->addWidget(new QLabel("関連語", this));

    Relations = new QTableWidget(this);
    layout->addWidget(Relations);
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
                         std::optional<std::string> place,
                         std::optional<int> period,
                         std::optional<int> wordID)
{
    Languages = languages;
    Place = place;
    Period = period;
    WordID = wordID;
    UpdateDialog();
}

/**
 * @brief ダイアログ更新
 *
 */
void EditWordDialog::UpdateDialog()
{
    if (Languages && Place && Period && WordID)
    {
        auto simulator = LanguageFamilySimulator::Create(*Languages);
        if (!simulator)
        {
            return;
        }
        auto language = simulator->CalculateLanguage(*Place, *Period);
        if (!language)
        {
            return;
        }
        PhonemeConverter converter = PhonemeConverter::Create(Languages->PhonemeTable);

        // 語形
        const auto form = language->Words[*WordID].Form;
        Entry->setText(QString::fromStdString(converter.ConvertToString(form)));

        // 訳語
        std::vector<std::pair<std::string, std::string>> translations;
        for (const auto &[title, forms] : language->Words[*WordID].Translations)
        {
            std::string formsStr = JoinStrs(forms, ",");
            translations.emplace_back(title, formsStr);
        }
        translations.emplace_back("", "");
        DisplayTranslations(TranslationLayout, translations);

        // タグ
        const auto tags = language->Words[*WordID].Tags;
        DisplayTable(Tags, {tags});

        // 自由記述
        const auto contents = language->Words[*WordID].Contents;
        std::vector<std::vector<std::string>> contentsData;
        std::vector<std::string> contentsLine;
        for (const auto &[title, content] : contents)
        {

            contentsLine.emplace_back(title);
            contentsLine.emplace_back(content);

            contentsData.emplace_back(contentsLine);
            contentsLine.clear();
        }
        DisplayTable(Contents, contentsData);

        // 変化形
        const auto variations = language->Words[*WordID].Variations;
        std::vector<std::vector<std::string>> variationsData;
        std::vector<std::string> variationsLine;
        for (const auto &[title, variation] : variations)
        {

            variationsLine.emplace_back(title);
            variationsLine.emplace_back(converter.ConvertToString(variation));

            variationsData.emplace_back(variationsLine);
            variationsLine.clear();
        }
        DisplayTable(Variations, variationsData);

        // 関連語
        const auto relations = language->Words[*WordID].Relations;
        std::vector<std::vector<std::string>> relationsData;
        std::vector<std::string> relationsLine;
        for (const auto &[title, relatedWordID] : relations)
        {

            relationsLine.emplace_back(title);
            relationsLine.emplace_back(converter.ConvertToString(language->Words[relatedWordID].Form));

            relationsData.emplace_back(relationsLine);
            relationsLine.clear();
        }
        DisplayTable(Relations, relationsData);
    }
}

/**
 * @brief 訳語を表示
 *
 * @param layout
 * @param translations
 */
void EditWordDialog::DisplayTranslations(QVBoxLayout *layout, const std::vector<std::pair<std::string, std::string>> &translations)
{
    ClearLayout(layout);
    for (const auto &[title, value] : translations)
    {
        QHBoxLayout *subLayout = new QHBoxLayout(this);

        auto titleLine = new QLineEdit(this);
        titleLine->setText(QString::fromStdString(title));
        subLayout->addWidget(titleLine);
        auto valueLine = new QLineEdit(this);
        valueLine->setText(QString::fromStdString(value));
        subLayout->addWidget(valueLine);

        layout->addLayout(subLayout);
    }
}
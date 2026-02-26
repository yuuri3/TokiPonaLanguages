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

    Translations = new QWidget(this);
    layout->addWidget(Translations);

    std::vector<std::vector<std::string>> translations = {{"", ""}};
    DisplayLine(Translations, translations, TWO_WIDTHS);

    //   * 訳語追加ボタン
    AddTranslationButton = new QPushButton("追加", this);
    AddTranslationButton->setFixedWidth(BUTTON_WIDTH);
    connect(AddTranslationButton, &QPushButton::clicked, this, &EditWordDialog::AddTranslationButtonPushed);
    translationsTitleLayout->addWidget(AddTranslationButton);

    // * タグ
    QHBoxLayout *tagsTitleLayout = new QHBoxLayout(this);
    tagsTitleLayout->addWidget(new QLabel("タグ", this));
    layout->addLayout(tagsTitleLayout);

    Tags = new QWidget(this);
    layout->addWidget(Tags);

    std::vector<std::vector<std::string>> tags = {{""}};
    DisplayLine(Tags, tags, ONE_WIDTH);

    //   * タグ追加ボタン
    AddTagsButton = new QPushButton("追加", this);
    AddTagsButton->setFixedWidth(BUTTON_WIDTH);
    connect(AddTagsButton, &QPushButton::clicked, this, &EditWordDialog::AddTagsButtonPushed);
    tagsTitleLayout->addWidget(AddTagsButton);

    // * 自由記述
    QHBoxLayout *contentsTitleLayout = new QHBoxLayout(this);
    contentsTitleLayout->addWidget(new QLabel("自由記述", this));
    layout->addLayout(contentsTitleLayout);

    Contents = new QWidget(this);
    layout->addWidget(Contents);

    std::vector<std::vector<std::string>> contents = {{"", ""}};
    DisplayLine(Contents, contents, TWO_WIDTHS);

    //   * 自由記述追加ボタン
    AddContentsButton = new QPushButton("追加", this);
    AddContentsButton->setFixedWidth(BUTTON_WIDTH);
    connect(AddContentsButton, &QPushButton::clicked, this, &EditWordDialog::AddContentsButtonPushed);
    contentsTitleLayout->addWidget(AddContentsButton);

    // * 変化形
    QHBoxLayout *variationsTitleLayout = new QHBoxLayout(this);
    variationsTitleLayout->addWidget(new QLabel("変化形", this));
    layout->addLayout(variationsTitleLayout);

    Variations = new QWidget(this);
    layout->addWidget(Variations);

    std::vector<std::vector<std::string>> variations = {{"", ""}};
    DisplayLine(Variations, variations, TWO_WIDTHS);

    //   * 変化形追加ボタン
    AddVariationsButton = new QPushButton("追加", this);
    AddVariationsButton->setFixedWidth(BUTTON_WIDTH);
    connect(AddVariationsButton, &QPushButton::clicked, this, &EditWordDialog::AddVariationsButtonPushed);
    variationsTitleLayout->addWidget(AddVariationsButton);

    // * 関連語
    QHBoxLayout *RelationsTitleLayout = new QHBoxLayout(this);
    RelationsTitleLayout->addWidget(new QLabel("関連語", this));
    layout->addLayout(RelationsTitleLayout);

    Relations = new QWidget(this);
    layout->addWidget(Relations);

    std::vector<std::vector<std::string>> relations = {{"", ""}};
    DisplayLine(Relations, relations, TWO_WIDTHS);

    //   * 関連語追加ボタン
    AddRelationsButton = new QPushButton("追加", this);
    connect(AddRelationsButton, &QPushButton::clicked, this, &EditWordDialog::AddRelationsButtonPushed);
    RelationsTitleLayout->addWidget(AddRelationsButton);
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
        std::vector<std::vector<std::string>> translations;
        for (const auto &[title, forms] : language->Words[*WordID].Translations)
        {
            std::string formsStr = JoinStrs(forms, ",");
            translations.push_back({title, formsStr});
        }
        translations.push_back({"", ""});
        DisplayLine(Translations, translations, TWO_WIDTHS);

        // タグ
        std::vector<std::vector<std::string>> tags;
        for (const auto &tag : language->Words[*WordID].Tags)
        {
            tags.push_back({tag});
        }
        tags.push_back({""});
        DisplayLine(Tags, tags, ONE_WIDTH);

        // 自由記述
        std::vector<std::vector<std::string>> contentsData;
        for (const auto &[title, content] : language->Words[*WordID].Contents)
        {
            contentsData.push_back({title, content});
        }
        contentsData.push_back({"", ""});
        DisplayLine(Contents, contentsData, TWO_WIDTHS);

        // 変化形
        std::vector<std::vector<std::string>> variationsData;
        for (const auto &[title, variation] : language->Words[*WordID].Variations)
        {
            variationsData.push_back({title, converter.ConvertToString(variation)});
        }
        variationsData.push_back({"", ""});
        DisplayLine(Variations, variationsData, TWO_WIDTHS);

        // 関連語
        std::vector<std::vector<std::string>> relationsData;
        for (const auto &[title, relatedWordID] : language->Words[*WordID].Relations)
        {
            if (language->Words.count(relatedWordID) == 1)
            {
                relationsData.push_back({title, converter.ConvertToString(language->Words[relatedWordID].Form)});
            }
        }
        relationsData.push_back({"", ""});
        DisplayLine(Relations, relationsData, TWO_WIDTHS);
    }
}

/**
 * @brief 訳語を表示
 *
 * @param layout
 * @param translations
 */
void EditWordDialog::DisplayLine(QWidget *widget, const std::vector<std::vector<std::string>> &values, const std::vector<int> &widths)
{
    ClearWidget(widget);
    for (const auto &line : values)
    {
        AddLine(widget, line, widths);
    }
}

/**
 * @brief 訳語行追加
 *
 */
void EditWordDialog::AddTranslationButtonPushed()
{
    AddLine(Translations, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief タグ追加ボタンクリック
 *
 */
void EditWordDialog::AddTagsButtonPushed()
{
    AddLine(Tags, {""}, ONE_WIDTH);
}

/**
 * @brief 自由記述追加ボタンクリック
 *
 */
void EditWordDialog::AddContentsButtonPushed()
{
    AddLine(Contents, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 変化形追加ボタンクリック
 *
 */
void EditWordDialog::AddVariationsButtonPushed()
{
    AddLine(Variations, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 関連語追加ボタンクリック
 *
 */
void EditWordDialog::AddRelationsButtonPushed()
{
    AddLine(Relations, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief レイアウトに2行追加
 *
 */
void EditWordDialog::AddLine(QWidget *widget, const std::vector<std::string> &values, const std::vector<int> &widths)
{
    if (!widget->layout())
    {
        widget->setLayout(new QVBoxLayout(widget));
    }

    QWidget *rowContainer = new QWidget(this);
    QHBoxLayout *subLayout = new QHBoxLayout(rowContainer);
    subLayout->setContentsMargins(0, 0, 0, 0);
    subLayout->setAlignment(Qt::AlignLeft);

    for (int i = 0; i < values.size(); i++)
    {
        const auto value = values[i];
        const auto width = widths[i];

        auto line = new QLineEdit(this);
        line->setText(QString::fromStdString(""));
        line->setFixedWidth(width);
        line->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(line, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
        subLayout->addWidget(line);
    }

    widget->layout()->addWidget(rowContainer);
}

/**
 * @brief 訳語編集メニューを表示
 *
 */
void EditWordDialog::ClickLine(const QPoint &pos)
{
    // 送信元（右クリックされたQLineEdit）を取得
    QLineEdit *senderLineEdit = qobject_cast<QLineEdit *>(sender());
    if (!senderLineEdit)
        return;

    QMenu menu(this);
    QAction *addAction = menu.addAction("追加");
    QAction *removeAction = menu.addAction("削除");

    QAction *selectedAction = menu.exec(senderLineEdit->mapToGlobal(pos));

    if (selectedAction == addAction)
    {
        int rowCount = senderLineEdit->parentWidget()->layout()->count();
        auto widths = (rowCount == 1) ? ONE_WIDTH : TWO_WIDTHS;
        std::vector<std::string> newLine(rowCount, "");
        AddLine(qobject_cast<QWidget *>(senderLineEdit->parent()->parent()), newLine, widths);
    }
    else if (selectedAction == removeAction)
    {
        QWidget *rowContainer = senderLineEdit->parentWidget();

        if (rowContainer && rowContainer != this)
        {
            DeleteWidget(rowContainer);
        }
    }
}
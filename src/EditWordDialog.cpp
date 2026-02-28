#include "EditWordDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamilySimulator.h"

EditWordDialog::EditWordDialog(QWidget *parent)
{
    setWindowTitle("単語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // * 見出し語
    layout->addWidget(new QLabel("見出し語", this));

    Entry_ = new QLineEdit(this);
    Entry_->setText("テスト単語");
    layout->addWidget(Entry_);

    // * 訳語
    QHBoxLayout *translationsTitleLayout = new QHBoxLayout(this);
    translationsTitleLayout->addWidget(new QLabel("訳語", this));
    layout->addLayout(translationsTitleLayout);

    Translations_ = new QWidget(this);
    layout->addWidget(Translations_);

    std::vector<std::vector<std::string>> translations = {{"", ""}};

    //   * 訳語追加ボタン
    AddTranslationButton_ = new QPushButton("追加", this);
    AddTranslationButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddTranslationButton_, &QPushButton::clicked, this, &EditWordDialog::AddTranslationButtonPushed);
    translationsTitleLayout->addWidget(AddTranslationButton_);

    // * タグ
    QHBoxLayout *tagsTitleLayout = new QHBoxLayout(this);
    tagsTitleLayout->addWidget(new QLabel("タグ", this));
    layout->addLayout(tagsTitleLayout);

    Tags_ = new QWidget(this);
    layout->addWidget(Tags_);

    std::vector<std::vector<std::string>> tags = {{""}};

    //   * タグ追加ボタン
    AddTagsButton_ = new QPushButton("追加", this);
    AddTagsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddTagsButton_, &QPushButton::clicked, this, &EditWordDialog::AddTagsButtonPushed);
    tagsTitleLayout->addWidget(AddTagsButton_);

    // * 自由記述
    QHBoxLayout *contentsTitleLayout = new QHBoxLayout(this);
    contentsTitleLayout->addWidget(new QLabel("自由記述", this));
    layout->addLayout(contentsTitleLayout);

    Contents_ = new QWidget(this);
    layout->addWidget(Contents_);

    std::vector<std::vector<std::string>> contents = {{"", ""}};

    //   * 自由記述追加ボタン
    AddContentsButton_ = new QPushButton("追加", this);
    AddContentsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddContentsButton_, &QPushButton::clicked, this, &EditWordDialog::AddContentsButtonPushed);
    contentsTitleLayout->addWidget(AddContentsButton_);

    // * 変化形
    QHBoxLayout *variationsTitleLayout = new QHBoxLayout(this);
    variationsTitleLayout->addWidget(new QLabel("変化形", this));
    layout->addLayout(variationsTitleLayout);

    Variations_ = new QWidget(this);
    layout->addWidget(Variations_);

    std::vector<std::vector<std::string>> variations = {{"", ""}};

    //   * 変化形追加ボタン
    AddVariationsButton_ = new QPushButton("追加", this);
    AddVariationsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddVariationsButton_, &QPushButton::clicked, this, &EditWordDialog::AddVariationsButtonPushed);
    variationsTitleLayout->addWidget(AddVariationsButton_);

    // * 関連語
    QHBoxLayout *RelationsTitleLayout = new QHBoxLayout(this);
    RelationsTitleLayout->addWidget(new QLabel("関連語", this));
    layout->addLayout(RelationsTitleLayout);

    Relations_ = new QWidget(this);
    layout->addWidget(Relations_);

    std::vector<std::vector<std::string>> relations = {{"", ""}};

    //   * 関連語追加ボタン
    AddRelationsButton_ = new QPushButton("追加", this);
    connect(AddRelationsButton_, &QPushButton::clicked, this, &EditWordDialog::AddRelationsButtonPushed);
    RelationsTitleLayout->addWidget(AddRelationsButton_);
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
void EditWordDialog::Set(const LanguageFamily &languages,
                         const std::string &place,
                         const int period,
                         const int wordID)
{
    Languages_ = languages;
    Place_ = place;
    Period_ = period;
    WordID_ = wordID;
    UpdateDialog();
}

/**
 * @brief 選択した言語をセット
 *
 * @param language
 */
void EditWordDialog::SetLanguage(const Language &language)
{
    Language_ = language;
    UpdateDialog();
}

/**
 * @brief ダイアログ更新
 *
 */
void EditWordDialog::UpdateDialog()
{
    if (Languages_ && Place_ && Period_ && WordID_)
    {
        std::optional<Language> language;
        if (Language_)
        {
            language = Language_;
        }
        else
        {
            auto simulator = LanguageFamilySimulator::Create(*Languages_);
            if (!simulator)
            {
                return;
            }
            language = simulator->CalculateLanguage(*Place_, *Period_);
            if (!language)
            {
                return;
            }
        }
        PhonemeConverter converter = PhonemeConverter::Create(Languages_->GetPhonemeTable());

        const auto word = language->GetWord(*WordID_);
        if (!word)
        {
            return;
        }

        // 語形
        const auto form = word->GetForm();
        Entry_->setText(QString::fromStdString(converter.ConvertToString(form)));

        // 訳語
        std::vector<std::vector<std::string>> translations;
        for (const auto &[title, forms] : word->GetTranslations())
        {
            std::string formsStr = JoinStrs(forms, ",");
            translations.push_back({title, formsStr});
        }
        translations.push_back({"", ""});
        DisplayLine(Translations_, translations, TWO_WIDTHS);

        // タグ
        std::vector<std::vector<std::string>> tags;
        for (const auto &tag : word->GetTags())
        {
            tags.push_back({tag});
        }
        tags.push_back({""});
        DisplayLine(Tags_, tags, ONE_WIDTH);

        // 自由記述
        std::vector<std::vector<std::string>> contentsData;
        for (const auto &[title, content] : word->GetContents())
        {
            contentsData.push_back({title, content});
        }
        contentsData.push_back({"", ""});
        DisplayLine(Contents_, contentsData, TWO_WIDTHS);

        // 変化形
        std::vector<std::vector<std::string>> variationsData;
        for (const auto &[title, variation] : word->GetVariations())
        {
            variationsData.push_back({title, converter.ConvertToString(variation)});
        }
        variationsData.push_back({"", ""});
        DisplayLine(Variations_, variationsData, TWO_WIDTHS);

        // 関連語
        std::vector<std::vector<std::string>> relationsData;
        for (const auto &[title, relatedWordID] : word->GetRealtions())
        {
            const auto relatedWord = language->GetWord(relatedWordID);
            if (relatedWord)
            {
                relationsData.push_back({title, converter.ConvertToString(relatedWord->GetForm())});
            }
        }
        relationsData.push_back({"", ""});
        DisplayLine(Relations_, relationsData, TWO_WIDTHS);
    }
}

/**
 * @brief 行を表示
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
    AddLine(Translations_, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief タグ追加ボタンクリック
 *
 */
void EditWordDialog::AddTagsButtonPushed()
{
    AddLine(Tags_, {""}, ONE_WIDTH);
}

/**
 * @brief 自由記述追加ボタンクリック
 *
 */
void EditWordDialog::AddContentsButtonPushed()
{
    AddLine(Contents_, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 変化形追加ボタンクリック
 *
 */
void EditWordDialog::AddVariationsButtonPushed()
{
    AddLine(Variations_, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 関連語追加ボタンクリック
 *
 */
void EditWordDialog::AddRelationsButtonPushed()
{
    AddLine(Relations_, {"", ""}, TWO_WIDTHS);
}

/**
 * @brief 行追加
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
 * @brief 行クリックイベント
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
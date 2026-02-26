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

    std::vector<std::pair<std::string, std::string>> translations = {{"", ""}};
    Display2Cols(Translations, translations);

    //   * 訳語追加ボタン
    AddTranslationButton = new QPushButton("訳語追加", this);
    connect(AddTranslationButton, &QPushButton::clicked, this, &EditWordDialog::AddTranslationButtonPushed);
    translationsTitleLayout->addWidget(AddTranslationButton);

    // * タグ
    QHBoxLayout *tagsTitleLayout = new QHBoxLayout(this);
    tagsTitleLayout->addWidget(new QLabel("タグ", this));
    layout->addLayout(tagsTitleLayout);

    Tags = new QWidget(this);
    layout->addWidget(Tags);

    std::vector<std::string> tags = {""};
    Display1Col(Tags, tags);

    //   * タグ追加ボタン
    AddTagsButton = new QPushButton("タグ追加", this);
    connect(AddTagsButton, &QPushButton::clicked, this, &EditWordDialog::AddTagsButtonPushed);
    tagsTitleLayout->addWidget(AddTagsButton);

    // * 自由記述
    QHBoxLayout *contentsTitleLayout = new QHBoxLayout(this);
    contentsTitleLayout->addWidget(new QLabel("自由記述", this));
    layout->addLayout(contentsTitleLayout);

    Contents = new QWidget(this);
    layout->addWidget(Contents);

    std::vector<std::pair<std::string, std::string>> contents = {{"", ""}};
    Display2Cols(Contents, contents);

    //   * 自由記述追加ボタン
    AddContentsButton = new QPushButton("自由記述追加", this);
    connect(AddContentsButton, &QPushButton::clicked, this, &EditWordDialog::AddContentsButtonPushed);
    contentsTitleLayout->addWidget(AddContentsButton);

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
        Display2Cols(Translations, translations);

        // タグ
        auto tags = language->Words[*WordID].Tags;
        tags.emplace_back("");
        Display1Col(Tags, tags);

        // 自由記述
        std::vector<std::pair<std::string, std::string>> contentsData;
        for (const auto &[title, content] : language->Words[*WordID].Contents)
        {
            contentsData.emplace_back(title, content);
        }
        contentsData.emplace_back("", "");
        Display2Cols(Contents, contentsData);

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
 * @brief 1列のデータを表示
 *
 * @param layout
 * @param values
 */
void EditWordDialog::Display1Col(QWidget *widget, std::vector<std::string> values)
{
    ClearWidget(widget);
    for (const auto &line : values)
    {
        Add1Col(widget, line);
    }
}

/**
 * @brief 訳語を表示
 *
 * @param layout
 * @param translations
 */
void EditWordDialog::Display2Cols(QWidget *widget, const std::vector<std::pair<std::string, std::string>> &translations)
{
    ClearWidget(widget);
    for (const auto &[title, value] : translations)
    {
        Add2Cols(widget, title, value);
    }
}

/**
 * @brief 訳語行追加
 *
 */
void EditWordDialog::AddTranslationButtonPushed()
{
    Add2Cols(Translations, "", "");
}

/**
 * @brief タグ追加ボタンクリック
 *
 */
void EditWordDialog::AddTagsButtonPushed()
{
    Add1Col(Tags, "");
}

/**
 * @brief 自由記述追加ボタンクリック
 *
 */
void EditWordDialog::AddContentsButtonPushed()
{
    Add2Cols(Contents, "", "");
}

/**
 * @brief レイアウトに行追加
 *
 * @param layout
 * @param value
 */
void EditWordDialog::Add1Col(QWidget *widget, std::string value)
{
    if (!widget->layout())
    {
        widget->setLayout(new QVBoxLayout(widget));
    }

    QLineEdit *lineEdit = new QLineEdit(this);
    lineEdit->setText(QString::fromStdString(value));
    lineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(lineEdit, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::Click1Col);
    widget->layout()->addWidget(lineEdit);
}

/**
 * @brief レイアウトに2行追加
 *
 */
void EditWordDialog::Add2Cols(QWidget *widget, std::string title, std::string value)
{
    if (!widget->layout())
    {
        widget->setLayout(new QVBoxLayout(widget));
    }

    QWidget *rowContainer = new QWidget(this);
    QHBoxLayout *subLayout = new QHBoxLayout(rowContainer);
    subLayout->setContentsMargins(0, 0, 0, 0);

    auto titleLine = new QLineEdit(this);
    titleLine->setText(QString::fromStdString(""));
    titleLine->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(titleLine, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::Click2Cols);
    subLayout->addWidget(titleLine);

    auto valueLine = new QLineEdit(this);
    valueLine->setText(QString::fromStdString(""));
    subLayout->addWidget(valueLine);
    valueLine->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(valueLine, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::Click2Cols);
    subLayout->addWidget(valueLine);

    widget->layout()->addWidget(rowContainer);
}

void EditWordDialog::Click1Col(const QPoint &pos)
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
        Add1Col(qobject_cast<QWidget *>(senderLineEdit->parent()), "");
    }
    else if (selectedAction == removeAction)
    {
        DeleteWidget(senderLineEdit);
    }
}

/**
 * @brief 訳語編集メニューを表示
 *
 */
void EditWordDialog::Click2Cols(const QPoint &pos)
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
        Add2Cols(qobject_cast<QWidget *>(senderLineEdit->parent()->parent()), "", "");
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
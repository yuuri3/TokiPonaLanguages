#include "EditWordDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamilySimulator.h"

EditWordDialog::EditWordDialog(QWidget *parent)
{
    setWindowTitle("単語編集");

    // メインレイアウト
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // スクロールエリアの設定
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);       // 中身のサイズに合わせて伸縮させる
    scrollArea->setFrameShape(QFrame::NoFrame); // 枠線を消してダイアログになじませる
    mainLayout->addWidget(scrollArea);

    // スクロールエリアの中身となるメインウィジェット
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // * 見出し語
    layout->addWidget(new QLabel("見出し語", scrollContent));

    Entry_ = new QLineEdit(scrollContent);
    Entry_->setText("テスト単語");
    layout->addWidget(Entry_);

    // * 訳語
    QHBoxLayout *translationsTitleLayout = new QHBoxLayout();
    translationsTitleLayout->addWidget(new QLabel("訳語", scrollContent));
    layout->addLayout(translationsTitleLayout);

    Translations_ = new QWidget(scrollContent);
    layout->addWidget(Translations_);

    //   * 訳語追加ボタン
    AddTranslationButton_ = new QPushButton("追加", scrollContent);
    AddTranslationButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddTranslationButton_, &QPushButton::clicked, this, &EditWordDialog::AddTranslationButtonPushed);
    translationsTitleLayout->addWidget(AddTranslationButton_);

    // * タグ
    QHBoxLayout *tagsTitleLayout = new QHBoxLayout();
    tagsTitleLayout->addWidget(new QLabel("タグ", scrollContent));
    layout->addLayout(tagsTitleLayout);

    Tags_ = new QWidget(scrollContent);
    layout->addWidget(Tags_);

    //   * タグ追加ボタン
    AddTagsButton_ = new QPushButton("追加", scrollContent);
    AddTagsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddTagsButton_, &QPushButton::clicked, this, &EditWordDialog::AddTagsButtonPushed);
    tagsTitleLayout->addWidget(AddTagsButton_);

    // * 自由記述
    QHBoxLayout *contentsTitleLayout = new QHBoxLayout();
    contentsTitleLayout->addWidget(new QLabel("自由記述", scrollContent));
    layout->addLayout(contentsTitleLayout);

    Contents_ = new QWidget(scrollContent);
    layout->addWidget(Contents_);

    //   * 自由記述追加ボタン
    AddContentsButton_ = new QPushButton("追加", scrollContent);
    AddContentsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddContentsButton_, &QPushButton::clicked, this, &EditWordDialog::AddContentsButtonPushed);
    contentsTitleLayout->addWidget(AddContentsButton_);

    // * 変化形
    QHBoxLayout *variationsTitleLayout = new QHBoxLayout();
    variationsTitleLayout->addWidget(new QLabel("変化形", scrollContent));
    layout->addLayout(variationsTitleLayout);

    Variations_ = new QWidget(scrollContent);
    layout->addWidget(Variations_);

    //   * 変化形追加ボタン
    AddVariationsButton_ = new QPushButton("追加", scrollContent);
    AddVariationsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddVariationsButton_, &QPushButton::clicked, this, &EditWordDialog::AddVariationsButtonPushed);
    variationsTitleLayout->addWidget(AddVariationsButton_);

    // * 関連語
    QHBoxLayout *RelationsTitleLayout = new QHBoxLayout();
    RelationsTitleLayout->addWidget(new QLabel("関連語", scrollContent));
    layout->addLayout(RelationsTitleLayout);

    Relations_ = new QWidget(scrollContent);
    layout->addWidget(Relations_);

    //   * 関連語追加ボタン
    AddRelationsButton_ = new QPushButton("追加", scrollContent);
    AddRelationsButton_->setFixedWidth(BUTTON_WIDTH);
    connect(AddRelationsButton_, &QPushButton::clicked, this, &EditWordDialog::AddRelationsButtonPushed);
    RelationsTitleLayout->addWidget(AddRelationsButton_);

    // 下部に余白を追加して上寄せにする
    layout->addStretch();
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
            const auto relatedWord = Language_.GetWord(relatedWordID);
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
 * @param widget 表示対象ウィジェット
 * @param values 値のリスト
 * @param widths 各列の幅
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
 * @param widget 親ウィジェット
 * @param values 入力値（[0]: タイトル, [1]: 内容）
 * @param widths 幅のリスト
 */
void EditWordDialog::AddLine(QWidget *widget, const std::vector<std::string> &values, const std::vector<int> &widths)
{
    if (!widget->layout())
    {
        widget->setLayout(new QVBoxLayout(widget));
        widget->layout()->setContentsMargins(0, 0, 0, 0);
    }

    QWidget *rowContainer = new QWidget(widget);
    QHBoxLayout *subLayout = new QHBoxLayout(rowContainer);
    subLayout->setContentsMargins(0, 0, 0, 0);
    subLayout->setAlignment(Qt::AlignTop); // 複数行入力時に左側のボックスが上に寄るように設定

    for (size_t i = 0; i < values.size(); i++)
    {
        // 自由記述(Contents_)セクションの2番目のボックスのみ QTextEdit を使用
        if (widget == Contents_ && i == 1)
        {
            auto textEdit = new QTextEdit(rowContainer);
            textEdit->setPlainText(QString::fromStdString(values[i]));
            textEdit->setFixedWidth(widths[i]);
            textEdit->setFixedHeight(80); // 複数行用に高さを確保
            textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(textEdit, &QTextEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
            subLayout->addWidget(textEdit);
        }
        else
        {
            auto line = new QLineEdit(rowContainer);
            line->setText(QString::fromStdString(values[i]));
            line->setFixedWidth(widths[i]);
            line->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(line, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
            subLayout->addWidget(line);
        }
    }

    widget->layout()->addWidget(rowContainer);
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

        // 1つの rowContainer 内にある入力要素（QLineEdit と QTextEdit）の合計数を取得
        int boxCount = rowContainer->findChildren<QLineEdit *>().count() +
                       rowContainer->findChildren<QTextEdit *>().count();

        // ボックスの数に応じた幅設定を維持
        auto widths = (boxCount == 1) ? ONE_WIDTH : TWO_WIDTHS;
        std::vector<std::string> newValues(boxCount, "");
        AddLine(targetWidget, newValues, widths);
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
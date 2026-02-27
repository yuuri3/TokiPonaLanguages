#include "EditLanguageWindow.h"
#include "UnimplementedDialog.h"
#include "EditWordDialog.h"

EditLanguageWindow::EditLanguageWindow(QWidget *parent)
{
    setWindowTitle("個別言語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    //   * 単語表示
    MainTable_ = new QTableWidget(this);
    layout->addWidget(MainTable_);

    MainTable_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(MainTable_, &QTableWidget::customContextMenuRequested,
            this, &EditLanguageWindow::ShowContextMenu);

    //   * 検索バー
    QHBoxLayout *searchLayout = new QHBoxLayout();

    SearchLineEdit_ = new QLineEdit(this);
    SearchLineEdit_->setPlaceholderText("検索ワードを入力...");

    SearchButton_ = new QPushButton("検索", this);

    connect(SearchButton_, &QPushButton::clicked, this, &EditLanguageWindow::Unimplemented);

    searchLayout->addWidget(SearchLineEdit_);
    searchLayout->addWidget(SearchButton_);

    layout->addLayout(searchLayout);

    //   * 単語追加ボタン
    AddWordButton_ = new QPushButton("単語追加", this);

    connect(AddWordButton_, &QPushButton::clicked, this, &EditLanguageWindow::Unimplemented);
    layout->addWidget(AddWordButton_);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditLanguageWindow::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditLanguageWindow::SetPlace(const std::string &place)
{
    Place_ = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditLanguageWindow::SetPeriod(const int period)
{
    Period_ = period;
    UpdateTable();
}

/**
 * @brief 表更新
 *
 */
void EditLanguageWindow::UpdateTable()
{
    if (Languages_ && Place_ && Period_)
    {
        auto simulator = LanguageFamilySimulator::Create(*Languages_);
        if (!simulator)
        {
            return;
        }
        auto language = simulator->CalculateLanguage(*Place_, *Period_);
        if (!language)
        {
            return;
        }

        std::vector<std::vector<std::string>> wordData;
        std::vector<std::string> line;
        PhonemeConverter converter = PhonemeConverter::Create(Languages_->PhonemeTable_);

        line.emplace_back("単語");
        line.emplace_back("訳語");
        wordData.emplace_back(line);
        line.clear();

        for (const auto &[ID, word] : language->Words_)
        {
            line.emplace_back(converter.ConvertToString(word.Form_));
            std::string translations;
            for (const auto [_, translation] : word.Translations_)
            {
                for (const auto &t : translation)
                {
                    translations += t;
                    translations += ",";
                }
            }
            line.emplace_back(translations);
            wordData.emplace_back(line);
            line.clear();
        }

        DisplayTable(MainTable_, wordData);
    }
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditLanguageWindow::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief 単語編集メニュー表示
 *
 */
void EditLanguageWindow::ShowContextMenu(const QPoint &pos)
{
    if (Languages_ && Place_ && Period_)
    {
        // クリックされた位置のアイテムを取得
        QTableWidgetItem *item = MainTable_->itemAt(pos);
        if (!item)
            return; // セルのない場所なら何もしない

        QMenu menu(this);
        QAction *editAction = menu.addAction("編集");

        // メニューを表示し、選ばれたアクションを取得
        QAction *selectedAction = menu.exec(MainTable_->viewport()->mapToGlobal(pos));

        if (selectedAction == editAction)
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
                Language_ = language;
                if (!language)
                {
                    return;
                }
            }

            const int row = MainTable_->currentRow();
            const int column = MainTable_->currentColumn();
            auto it = std::next(language->Words_.begin(), row);
            int wordID = it->first - 1;

            EditWordDialog subWindow(this);
            subWindow.SetLanguage(*language);
            subWindow.Set(*Languages_, *Place_, *Period_, wordID);
            subWindow.exec();
        }
    }
}
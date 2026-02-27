#include "EditLanguageWindow.h"
#include "UnimplementedDialog.h"
#include "EditWordDialog.h"

EditLanguageWindow::EditLanguageWindow(QWidget *parent)
{
    setWindowTitle("個別言語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    //   * 単語表示
    mainTable = new QTableWidget(this);
    layout->addWidget(mainTable);

    mainTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mainTable, &QTableWidget::customContextMenuRequested,
            this, &EditLanguageWindow::ShowContextMenu);

    //   * 検索バー
    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("検索ワードを入力...");

    searchButton = new QPushButton("検索", this);

    connect(searchButton, &QPushButton::clicked, this, &EditLanguageWindow::Unimplemented);

    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);

    layout->addLayout(searchLayout);

    //   * 単語追加ボタン
    addWordButton = new QPushButton("単語追加", this);

    connect(addWordButton, &QPushButton::clicked, this, &EditLanguageWindow::Unimplemented);
    layout->addWidget(addWordButton);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditLanguageWindow::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditLanguageWindow::SetPlace(const std::string &place)
{
    Place = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditLanguageWindow::SetPeriod(const int period)
{
    Period = period;
    UpdateTable();
}

/**
 * @brief 表更新
 *
 */
void EditLanguageWindow::UpdateTable()
{
    if (Languages && Place && Period)
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

        std::vector<std::vector<std::string>> wordData;
        std::vector<std::string> line;
        PhonemeConverter converter = PhonemeConverter::Create(Languages->PhonemeTable);

        line.emplace_back("単語");
        line.emplace_back("訳語");
        wordData.emplace_back(line);
        line.clear();

        for (const auto &[ID, word] : language->Words)
        {
            line.emplace_back(converter.ConvertToString(word.Form));
            std::string translations;
            for (const auto [_, translation] : word.Translations)
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

        DisplayTable(mainTable, wordData);
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
    if (Languages && Place && Period)
    {
        // クリックされた位置のアイテムを取得
        QTableWidgetItem *item = mainTable->itemAt(pos);
        if (!item)
            return; // セルのない場所なら何もしない

        QMenu menu(this);
        QAction *editAction = menu.addAction("編集");

        // メニューを表示し、選ばれたアクションを取得
        QAction *selectedAction = menu.exec(mainTable->viewport()->mapToGlobal(pos));

        if (selectedAction == editAction)
        {
            std::optional<Language> language;
            if (Language_)
            {
                language = Language_;
            }
            else
            {
                auto simulator = LanguageFamilySimulator::Create(*Languages);
                if (!simulator)
                {
                    return;
                }
                language = simulator->CalculateLanguage(*Place, *Period);
                Language_ = language;
                if (!language)
                {
                    return;
                }
            }

            const int row = mainTable->currentRow();
            const int column = mainTable->currentColumn();
            auto it = std::next(language->Words.begin(), row);
            int wordID = it->first - 1;

            EditWordDialog subWindow(this);
            subWindow.SetLanguage(*language);
            subWindow.Set(*Languages, *Place, *Period, wordID);
            subWindow.exec();
        }
    }
}
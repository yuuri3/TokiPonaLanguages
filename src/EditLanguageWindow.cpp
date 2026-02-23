#include "EditLanguageWindow.h"
#include "UnimplementedDialog.h"

EditLanguageWindow::EditLanguageWindow(QWidget *parent)
{
    setWindowTitle("個別言語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    //   * 単語表示
    mainTable = new QTableWidget(this);
    layout->addWidget(mainTable);

    //   * 検索バー
    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("検索ワードを入力...");

    searchButton = new QPushButton("検索", this);

    connect(searchButton, &QPushButton::clicked, this, &EditLanguageWindow::Unimplemented);

    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);

    layout->addLayout(searchLayout);
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
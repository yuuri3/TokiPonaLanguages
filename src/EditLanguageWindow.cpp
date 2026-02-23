#include "EditLanguageWindow.h"

EditLanguageWindow::EditLanguageWindow(QWidget *parent)
{
    setWindowTitle("個別言語編集");

    QHBoxLayout *layout = new QHBoxLayout(this);

    //   * 単語表示
    mainTable = new QTableWidget(this);
    layout->addWidget(mainTable);
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
        for (const auto &[ID, word] : language->Words)
        {
            line.emplace_back(converter.ConvertToString(word.Form));
            wordData.emplace_back(line);
            line.clear();
        }

        DisplayTable(mainTable, wordData);
    }
}
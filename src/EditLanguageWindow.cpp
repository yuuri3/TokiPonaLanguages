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

        DisplayTable(wordData);
    }
}

/**
 * @brief 文字列の配列をウィンドウに表示
 *
 * @param window ウィンドウ
 * @param data 文字列の配列
 */
void EditLanguageWindow::DisplayTable(const std::vector<std::vector<std::string>> &data)
{
    mainTable->clear();
    mainTable->setRowCount(0);
    mainTable->setColumnCount(0);

    if (!data.empty())
    {
        int rows = data.size();
        int cols = data[0].size();
        mainTable->setRowCount(rows);
        mainTable->setColumnCount(cols);

        // 3. データの流し込み
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                // std::string から QString へ変換してセット
                QString content = QString::fromStdString(data[i][j]);
                mainTable->setItem(i, j, new QTableWidgetItem(content));
            }
        }
    }

    mainTable->verticalHeader()->setVisible(false);
    mainTable->horizontalHeader()->setVisible(false);
    mainTable->resizeColumnsToContents();
    mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    mainTable->resizeColumnsToContents();
}
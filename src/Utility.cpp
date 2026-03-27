#include "Utility.h"

/**
 * CSVファイルを読み込んで2次元ベクトルで返す関数
 */
std::vector<std::vector<std::string>> readCSV(const std::string &filename)
{
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename.c_str());

    if (!file.is_open())
    {
        std::cerr << "Error: ファイルを開けませんでした: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        // カンマ区切りで各セルを読み込む
        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }

        // 行が空でない場合にデータに追加
        if (!row.empty())
        {
            data.push_back(row);
        }
    }

    file.close();
    return data;
}

/**
 * 2次元ベクトルをCSVファイルに出力する関数
 * @return 保存に成功したらtrue、失敗したらfalse
 */
bool writeCSV(const std::string &filename, const std::vector<std::vector<std::string>> &data)
{
    std::ofstream file(filename.c_str());

    if (!file.is_open())
    {
        std::cerr << "Error: ファイルを開けませんでした: " << filename << std::endl;
        return false;
    }

    for (size_t row = 0; row < data.size(); ++row)
    {
        for (size_t column = 0; column < data[row].size(); ++column)
        {
            file << data[row][column];

            // 最後の列以外にはカンマを挿入する
            if (column < data[row].size() - 1)
            {
                file << ",";
            }
        }
        // 行の終わりに改行を挿入
        file << "\n";
    }

    file.close();
    return true;
}

/**
 * 2次元データから隣接関係（横・縦）を抽出する関数
 * @param data 2次元文字列ベクトル
 * @return 隣接する要素のペアのリスト
 */
std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data)
{
    std::vector<std::pair<std::string, std::string>> edges;

    for (size_t row = 0; row < data.size(); ++row)
    {
        for (size_t column = 0; column < data[row].size(); ++column)
        {
            // 空文字列はスルー
            if (data[row][column].empty())
            {
                continue;
            }
            // 1. 横方向の隣接（右隣があるかチェック）
            if (column + 1 < data[row].size() && !data[row][column + 1].empty())
            {
                edges.push_back({data[row][column], data[row][column + 1]});
            }

            // 2. 縦方向の隣接（一つ下の行に同じ列があるかチェック）
            if (row + 1 < data.size() && column < data[row + 1].size() && !data[row + 1][column].empty())
            {
                edges.push_back({data[row][column], data[row + 1][column]});
            }
        }
    }
    return edges;
}

/**
 * 2次元データから空ではない文字列のみを抽出して1次元のリストにする
 * @param data 2次元文字列ベクトル
 */
std::vector<std::string> getNonEmptyStrings(const std::vector<std::vector<std::string>> &data)
{
    std::vector<std::string> result;

    for (const auto &row : data)
    {
        for (const auto &cell : row)
        {
            // 文字列が空（""）でない場合のみ追加
            if (!cell.empty())
            {
                result.push_back(cell);
            }
        }
    }

    return result;
}

/**
 * @brief ウィンドウに表を表示
 *
 * @param table 表
 * @param data データ
 */
void DisplayTable(QTableWidget *table, const std::vector<std::vector<std::string>> &data, const bool IsEdit)
{
    constexpr int CELL_HEIGHT = 30;
    constexpr int CELL_WIDTH = 30;
    table->clear();
    table->setRowCount(0);
    table->setColumnCount(0);

    int rows = data.size();
    int cols = 0;

    for (const auto &row : data)
    {
        cols = std::max(cols, static_cast<int>(row.size()));
    }

    if (!data.empty())
    {
        table->setRowCount(rows);
        table->setColumnCount(cols);

        // 3. データの流し込み
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                if (j < data[i].size())
                {
                    // std::string から QString へ変換してセット
                    QString content = QString::fromStdString(data[i][j]);
                    table->setItem(i, j, new QTableWidgetItem(content));
                }
                else
                {
                    table->setItem(i, j, new QTableWidgetItem(""));
                }
            }
        }
    }

    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(CELL_HEIGHT);
    table->horizontalHeader()->setDefaultSectionSize(CELL_WIDTH);
    if (!IsEdit)
    {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    table->resizeColumnsToContents();
}

/**
 * @brief 文字列を結合
 *
 * @param elements 文字列の配列
 * @param delimiter 区切り文字
 * @return std::string
 */
std::string JoinStrs(const std::vector<std::string> &elements, const std::string &delimiter)
{
    std::stringstream ss;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        ss << elements[i];
        if (i != elements.size() - 1)
        {
            ss << delimiter; // 最後の要素以外に区切り文字を入れる
        }
    }
    return ss.str();
}

// 前後のスペースを除去。 スペースのみのセルは空文字列とする。
std::string EraseSpace(std::string str)
{
    size_t firstNoSpacePosotion = str.find_first_not_of(" ");
    size_t lastNoSpacePosition = str.find_last_not_of(" ");

    if (firstNoSpacePosotion != std::string::npos)
        return str.substr(firstNoSpacePosotion, (lastNoSpacePosition - firstNoSpacePosotion + 1));
    else
        return "";
}

/**
 * @brief レイアウトの中身を消去
 *
 * @param layout レイアウト
 */
void ClearLayout(QLayout *layout)
{
    if (!layout)
        return;

    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->setParent(nullptr);
            widget->deleteLater();
        }
        else if (QLayout *childLayout = item->layout())
        {
            ClearLayout(childLayout);
        }
        delete item;
    }
}

/**
 * @brief ウィジェットを安全に削除
 *
 * @param widget 削除対象のウィジェット
 */
void DeleteWidget(QWidget *widget)
{
    if (!widget)
        return;

    // 1. 親ウィジェットとの紐付けを解除して画面から消す
    widget->setParent(nullptr);

    // 2. イベントループの安全なタイミングでメモリを解放する
    widget->deleteLater();
}

/**
 * @brief レイアウトの中身を消去
 * * @param widget クリア対象のウィジェット
 */
void ClearWidget(QWidget *widget)
{
    if (!widget)
    {
        return;
    }

    // 既存のレイアウトを取得
    QLayout *oldLayout = widget->layout();

    if (oldLayout)
    {
        // 1. レイアウト内のウィジェットやサブレイアウトをすべて削除
        while (QLayoutItem *item = oldLayout->takeAt(0))
        {
            if (QWidget *childWidget = item->widget())
            {
                DeleteWidget(childWidget);
            }
            else if (QLayout *childLayout = item->layout())
            {
                // サブレイアウトがある場合は、その中身も再帰的にクリア
                ClearLayout(childLayout);
            }
            delete item;
        }

        // 2. レイアウト本体を削除して、ウィジェットから切り離す
        delete oldLayout;
    }
}

// YAMLの [a, b, c] 形式を vector<string> に変換する
const std::vector<std::string> ParseVector(const std::string &line)
{
    std::vector<std::string> result;

    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        result.push_back(item);
    }
    return result;
}

const std::vector<int> ParseIntVector(const std::string &line)
{
    std::vector<int> result;

    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        try
        {
            int param = std::stoi(EraseSpace(item));
            result.push_back(param);
        }
        catch (...)
        {
            return {};
        }
    }
    return result;
}

const std::vector<double> ParseDoubleVector(const std::string &line)
{
    std::vector<double> result;

    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        try
        {
            double param = std::stod(EraseSpace(item));
            result.push_back(param);
        }
        catch (...)
        {
            return {};
        }
    }
    return result;
}

/**
 * @brief json ファイル読み込み
 *
 * @param fileName
 * @return ImportData
 */
ImportData ImportFromJson(const QString &fileName)
{
    ImportData result;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        return result; // success = false のまま返す
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError)
    {
        return result;
    }

    QJsonObject root = doc.object();

    // データの抽出
    result.version = root["version"].toInt();
    result.words = root["words"].toArray();
    result.examples = root["examples"].toArray();
    result.success = true;

    return result;
}

/**
 * @brief 文字列と数値を ": " で結合する
 *
 * @param text 結合する文字列
 * @param value 結合する数値
 * @return std::string 結合後の文字列
 */
std::string JoinStringAndInt(const std::string &text, int value)
{
    return text + ": " + std::to_string(value);
}

/**
 * @brief 文字列を ": " で分割し、元の文字列と数値を取得する
 *
 * @param combinedString 結合された状態の文字列
 * @param outText 分割後の文字列を格納する変数
 * @param outValue 分割後の数値を格納する変数
 * @return bool 分割および数値変換に成功したかどうか
 */
bool ParseStringAndInt(const std::string &combinedString, const std::string &text, int &value)
{
    const std::string delimiter = ": ";
    size_t pos = combinedString.find(delimiter);
    if (pos == std::string::npos)
    {
        return false;
    }

    if (text != combinedString.substr(0, pos))
    {
        return false;
    }
    std::string valueStr = combinedString.substr(pos + delimiter.length());

    try
    {
        value = std::stoi(valueStr);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

/**
 * @brief std::pair<int, int> のハッシュ値を計算する
 * * @param p ハッシュ化する整数のペア
 * @return std::size_t 計算されたハッシュ値
 */
std::size_t PairHash::operator()(const std::pair<int, int> &p) const
{
    auto hash1 = std::hash<int>{}(p.first);
    auto hash2 = std::hash<int>{}(p.second);
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}
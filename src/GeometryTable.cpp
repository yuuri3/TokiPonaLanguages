#include "GeometryTable.h"
#include "Utility.h"

/**
 * @brief ジオメトリマスタデータ（場所名・グリッドマップ）を出力する
 * * @param file 出力ストリーム
 */
void GeometryTable::Export(std::ofstream &file) const
{
    // 1. PlaceName (ID, Name)
    file << JoinStringAndInt(SECTION_NAME_GEOGRAPHY_PLACE_NAME, static_cast<int>(PlaceName_.size())) << "\n";
    for (const auto &[id, name] : PlaceName_)
    {
        file << id << "," << name << "\n";
    }

    // 2. Grid Map (Key1, Key2 -> Value)
    file << JoinStringAndInt(SECTION_NAME_GEOGRAPHY_GRIDMAP, static_cast<int>(GridMap_.size())) << "\n";
    for (const auto &[pair, value] : GridMap_)
    {
        std::vector<int> gridRow = {pair.first, pair.second, value};
        file << FormatVector<int>(gridRow) << "\n";
    }
}

/**
 * @brief ジオメトリマスタデータを読み込む
 * * @param file 入力ストリーム
 * @return bool 成功したかどうか
 */
bool GeometryTable::Import(std::ifstream &file)
{
    std::string line;
    auto getNextLine = [&]()
    {
        if (std::getline(file, line))
            return true;
        else
            return false;
    };

    int count = 0;
    if (getNextLine() && ParseStringAndInt(line, SECTION_NAME_GEOGRAPHY_PLACE_NAME, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!getNextLine())
                return false;

            auto parts = ParseVector(line);
            PlaceName_[std::stoi(parts[0])] = parts[1];
        }
    }
    else
    {
        return false;
    }
    if (getNextLine() && ParseStringAndInt(line, SECTION_NAME_GEOGRAPHY_GRIDMAP, count))
    {
        // 要素がある場合のみベクトルを読み込む
        if (count > 0)
        {
            for (int i = 0; i < count; ++i)
            {
                if (!getNextLine())
                    return false;

                auto parts = ParseVector(line);
                GridMap_[{std::stoi(parts[0]), std::stoi(parts[1])}] = std::stoi(parts[2]);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

/**
 * @brief 2次元配列からジオメトリテーブルを初期化する
 * * @param geography 地理情報の2次元配列
 */
void GeometryTable::InitializeFromVector(const std::vector<std::vector<std::string>> &geography)
{
    PlaceName_.clear();
    GridMap_.clear();

    int placeId = 0;
    std::unordered_map<std::string, int> nameToIdMap;

    for (size_t y = 0; y < geography.size(); ++y)
    {
        for (size_t x = 0; x < geography[y].size(); ++x)
        {
            std::string placeName = geography[y][x];

            if (placeName.empty())
            {
                continue;
            }

            if (nameToIdMap.find(placeName) == nameToIdMap.end())
            {
                nameToIdMap[placeName] = placeId;
                PlaceName_[placeId] = placeName;
                placeId++;
            }

            GridMap_[{static_cast<int>(x), static_cast<int>(y)}] = nameToIdMap[placeName];
        }
    }
}

/**
 * @brief ジオメトリテーブルを2次元配列に変換する
 * * @return std::vector<std::vector<std::string>>
 */
std::vector<std::vector<std::string>> GeometryTable::ToVector() const
{
    int maxX = -1;
    int maxY = -1;

    for (const auto &[coord, id] : GridMap_)
    {
        if (coord.first > maxX)
        {
            maxX = coord.first;
        }
        if (coord.second > maxY)
        {
            maxY = coord.second;
        }
    }

    if (maxX == -1 || maxY == -1)
    {
        return {};
    }

    std::vector<std::vector<std::string>> result(maxY + 1, std::vector<std::string>(maxX + 1, ""));

    for (const auto &[coord, id] : GridMap_)
    {
        auto it = PlaceName_.find(id);
        if (it != PlaceName_.end())
        {
            result[coord.second][coord.first] = it->second;
        }
    }

    return result;
}

/**
 * @brief 指定した座標の地名を設定・更新する
 * * @param row 行
 * * @param column 列
 * * @param name 地名
 */
void GeometryTable::SetPlaceName(const int row, const int column, const std::string &name)
{
    if (name.empty())
    {
        GridMap_.erase({column, row});
        return;
    }

    int targetId = -1;
    for (const auto &[id, placeName] : PlaceName_)
    {
        if (placeName == name)
        {
            targetId = id;
            break;
        }
    }

    if (targetId == -1)
    {
        int maxId = -1;
        for (const auto &[id, placeName] : PlaceName_)
        {
            if (id > maxId)
            {
                maxId = id;
            }
        }
        targetId = maxId + 1;
        PlaceName_[targetId] = name;
    }

    GridMap_[{column, row}] = targetId;
}

/**
 * @brief 上行に空行を追加し、それ以降の行を下にシフトする
 * * @param row 行
 */
void GeometryTable::AddRowAbove(const int row)
{
    if (row < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (y >= row)
        {
            newGridMap[{x, y + 1}] = id;
        }
        else
        {
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief 下行に空行を追加し、それ以降の行を下にシフトする
 * * @param row 行
 */
void GeometryTable::AddRowBelow(const int row)
{
    if (row < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (y > row)
        {
            newGridMap[{x, y + 1}] = id;
        }
        else
        {
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief 指定した行を削除し、それ以降の行を上にシフトする
 * * @param row 行
 */
void GeometryTable::DeleteRow(const int row)
{
    if (row < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (y == row)
        {
            // 削除対象行のデータは移行しない
            continue;
        }
        else if (y > row)
        {
            // 削除された行より下のデータは1行上に詰める
            newGridMap[{x, y - 1}] = id;
        }
        else
        {
            // 削除された行より上のデータはそのまま
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief 指定列の右に空列を追加し、それ以降の列を右にシフトする
 * * @param column 列
 */
void GeometryTable::AddColumnRight(const int column)
{
    if (column < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (x > column)
        {
            newGridMap[{x + 1, y}] = id;
        }
        else
        {
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief 指定列の左に空列を追加し、それ以降の列を右にシフトする
 * * @param column 列
 */
void GeometryTable::AddColumnLeft(const int column)
{
    if (column < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (x >= column)
        {
            newGridMap[{x + 1, y}] = id;
        }
        else
        {
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief 指定した列を削除し、それ以降の列を左にシフトする
 * * @param column 列
 */
void GeometryTable::DeleteColumn(const int column)
{
    if (column < 0)
    {
        return;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> newGridMap;

    for (const auto &[coord, id] : GridMap_)
    {
        int x = coord.first;
        int y = coord.second;

        if (x == column)
        {
            continue;
        }
        else if (x > column)
        {
            newGridMap[{x - 1, y}] = id;
        }
        else
        {
            newGridMap[{x, y}] = id;
        }
    }

    GridMap_ = std::move(newGridMap);
}

/**
 * @brief ジオメトリテーブルが空かどうかを判定する
 * * @return bool 空の場合は true、そうでない場合は false
 */
bool GeometryTable::Empty() const
{
    return GridMap_.empty();
}
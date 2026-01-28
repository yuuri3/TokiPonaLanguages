#include "Utility.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<std::vector<std::string>> readCSV(const std::wstring &filename)
{
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename.c_str());

    if (!file.is_open())
    {
        std::wcerr << "Error: ファイルを開けませんでした: " << filename << std::endl;
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

bool writeCSV(const std::wstring &filename, const std::vector<std::vector<std::string>> &data)
{
    std::ofstream file(filename.c_str());

    if (!file.is_open())
    {
        std::wcerr << "Error: ファイルを開けませんでした: " << filename << std::endl;
        return false;
    }

    for (size_t i = 0; i < data.size(); ++i)
    {
        for (size_t j = 0; j < data[i].size(); ++j)
        {
            file << data[i][j];

            // 最後の列以外にはカンマを挿入する
            if (j < data[i].size() - 1)
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

std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data)
{
    std::vector<std::pair<std::string, std::string>> edges;

    for (size_t r = 0; r < data.size(); ++r)
    {
        for (size_t c = 0; c < data[r].size(); ++c)
        {
            // 空文字列はスルー
            if (data[r][c].empty())
            {
                continue;
            }
            // 1. 横方向の隣接（右隣があるかチェック）
            if (c + 1 < data[r].size() && !data[r][c + 1].empty())
            {
                edges.push_back({data[r][c], data[r][c + 1]});
            }

            // 2. 縦方向の隣接（一つ下の行に同じ列があるかチェック）
            if (r + 1 < data.size() && c < data[r + 1].size() && !data[r + 1][c].empty())
            {
                edges.push_back({data[r][c], data[r + 1][c]});
            }
        }
    }
    return edges;
}

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
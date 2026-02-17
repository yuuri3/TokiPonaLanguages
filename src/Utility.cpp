#include "Utility.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
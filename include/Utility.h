#pragma once

#include "stdafx.h"

struct Phoneme;

static std::string APPLICATION_NAME = "うなぎエディタ";
static std::string APPLICATION_VERSION = "0.4.1";

std::vector<std::vector<std::string>> readCSV(const std::string &filename);
bool writeCSV(const std::string &filename, const std::vector<std::vector<std::string>> &data);
std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data);
std::vector<std::string> getNonEmptyStrings(const std::vector<std::vector<std::string>> &data);
std::string JoinStrs(const std::vector<std::string> &elements, const std::string &delimiter);
std::string EraseSpace(std::string str);

void DisplayTable(QTableWidget *table, const std::vector<std::vector<std::string>> &data, const bool IsEdit = false);
void ClearLayout(QLayout *layout);
void DeleteWidget(QWidget *widget);
void ClearWidget(QWidget *widget);

template <typename T>

// ヘルパー関数：ベクトルをカンマ区切りのリスト形式にする
const std::string FormatVector(const std::vector<T> &vec)
{
    if (vec.empty())
        return ",";
    std::stringstream ss;
    ss << "";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        ss << vec[i] << ",";
    }
    return ss.str();
}

// ヘルパー関数：ベクトルをカンマ区切りのリスト形式にする
const std::string FormatPhonemesToVector(const std::vector<Phoneme> &vec);

const std::vector<std::string> ParseVector(const std::string &line);
const std::vector<int> ParseIntVector(const std::string &line);
const std::vector<double> ParseDoubleVector(const std::string &line);
const std::vector<Phoneme> ParsePhonemeVector(const std::string &line);

struct ImportData
{
    int version;
    QJsonArray words;
    QJsonArray examples;
    bool success = false;
};

ImportData ImportFromJson(const QString &fileName);
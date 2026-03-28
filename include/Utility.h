#pragma once

#include "stdafx.h"

struct Phoneme;

static const std::string APPLICATION_NAME = "うなぎエディタ";
static const std::string APPLICATION_VERSION = "0.5.0";

static const std::string SECTION_NAME_GEOGRAPHY = "GEO";
static const std::string SECTION_NAME_GEOGRAPHY_PLACE_NAME = "PLN";
static const std::string SECTION_NAME_GEOGRAPHY_GRIDMAP = "GGM";
static const std::string SECTION_NAME_PHONEMETABLE = "PHT";
static const std::string SECTION_NAME_PHONEMETABLE_FEATURE = "FET";
static const std::string SECTION_NAME_PHONEMETABLE_PLACE = "PLC";
static const std::string SECTION_NAME_PHONEMETABLE_MANNER = "MNR";
static const std::string SECTION_NAME_PHONEMETABLE_PHONEME = "PHN";
static const std::string SECTION_NAME_PHONEMETABLE_PLACE_FEATURE = "PLF";
static const std::string SECTION_NAME_PHONEMETABLE_MANNER_FEATURE = "MNF";
static const std::string SECTION_NAME_PHONEMETABLE_PHONEME_FEATURE = "PHF";
static const std::string SECTION_NAME_PHONEMETABLE_GRIDMAP = "GRM";
static const std::string SECTION_NAME_DIFFERENCES = "DIF";
static const std::string SECTION_NAME_DIFFERENCES_PERIOD = "PRD";
static const std::string SECTION_NAME_DIFFERENCES_TYPE = "TYP";
static const std::string SECTION_NAME_DIFFERENCES_INT_PARAM = "IPR";
static const std::string SECTION_NAME_DIFFERENCES_DOUBLE_PARAM = "DPR";
static const std::string SECTION_NAME_DIFFERENCES_STRING_PARAM = "SPR";
static const std::string SECTION_NAME_DIFFERENCES_FORM = "FRM";
static const std::string SECTION_NAME_DIFFERENCES_BEFORE_PHONEME = "BPH";
static const std::string SECTION_NAME_DIFFERENCES_AFTER_PHONEME = "APH";
static const std::string SECTION_NAME_DIFFERENCES_ENVIRONMENT = "ENV";

static const std::string DELIMINATOR_BEFORE_AFTER = " > ";
static const std::string DELIMINATOR_AFTER_ENV = " / ";
static const std::string DELIMINATOR_ENV_ENV = " _ ";

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

const std::vector<std::string> ParseVector(const std::string &line);
const std::vector<int> ParseIntVector(const std::string &line);
const std::vector<double> ParseDoubleVector(const std::string &line);

struct ImportData
{
    int version;
    QJsonArray words;
    QJsonArray examples;
    bool success = false;
};

ImportData ImportFromJson(const QString &fileName);

std::string JoinStringAndInt(const std::string &text, int value);
bool ParseStringAndInt(const std::string &combinedString, const std::string &text, int &value);

struct PairHash
{
    std::size_t operator()(const std::pair<int, int> &p) const;
};
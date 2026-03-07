#pragma once

#include "LanguageDifference.h"

class Language;

/**
 * @brief 語族
 *
 */
struct LanguageFamily
{
private:
    // 地理
    std::vector<std::vector<std::string>> Geography_;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable_;
    // 差分
    std::vector<LanguageDifference> languageDifference_;

public:
    static std::vector<std::vector<std::string>> RomanAlphabetTable;

    static LanguageFamily Create(const std::vector<std::vector<std::string>> &geography, const std::vector<std::vector<std::string>> &phonemeTable);
    const std::vector<std::vector<std::string>> &GetGeography() const;
    const std::vector<std::vector<std::string>> &GetPhonemeTable() const;
    void AddDifference(const LanguageDifference &languageDifference);

    void ChangePlaceName(const int row, const int column, const std::string &name);
    void AddGeomgraphicRowAbove(const int row);
    void AddGeomgraphicRowBelow(const int row);
    void DeleteGeomgraphicRow(const int row);
    void AddGeomgraphicColumnRight(const int column);
    void AddGeomgraphicColumnLeft(const int column);
    void DeleteGeomgraphicColumn(const int column);

    void AddPeriodAbove(const int period);
    void AddPeriodBelow(const int period);
    void RemovePeriod(const int period);

    const bool Empty() const;
    std::optional<Language> CalculateLanguage(const std::string place, const int period);
    const std::vector<std::vector<std::string>> ToString() const;

    void Export(const std::string &filename);
    bool Import(const std::string &filename);

    bool ImportJson(const std::string &filename);
};
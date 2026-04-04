#pragma once

#include "LanguageDifference.h"
#include "PhonemeTable.h"
#include "GeometryTable.h"
#include "PhonologicalChange.h"
#include "GeometryDifference.h"

class Language;

/**
 * @brief 語族
 *
 */
struct LanguageFamily
{
private:
    // 地理
    GeometryTable Geography_;
    // 音韻
    PhonemeTable PhonemeTable_;
    // 差分
    std::vector<LanguageDifference> languageDifference_;

public:
    static std::vector<std::vector<std::string>> RomanAlphabetTable;

    static LanguageFamily Create(const std::vector<std::vector<std::string>> &geography, const PhonemeTable &phonemeTable);
    std::vector<std::vector<std::string>> GetGeography() const;
    const PhonemeTable &GetPhonemeTable() const;
    void AddDifference(const LanguageDifference &languageDifference);

    void EditGeometry(const std::vector<GeometryDifference> &differences);

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

    const std::vector<PhonologicalChange> GetPhonologicalChanges(const std::string place, const int period);
    const std::vector<std::string> GetPhonologicalChangeStrings(const std::string place, const int period);
    bool SetPhonologicalChanges(const std::string place, const int period, const std::vector<PhonologicalChange> &phonologicalChange);
    bool SetPhonologicalChangesFromString(const std::string place, const int period, const std::vector<std::string> &phonologicalChange);

    std::vector<std::pair<int, int>> GetLoanwordIDs(const std::string &targetPlace, const std::string &referencePlace, const int period);
    bool SetLoanwords(const std::string &targetPlace, const std::string &referencePlace, const int period, const std::vector<std::pair<int, int>> &wordIDs);

    void Export(const std::string &filename);
    bool Import(const std::string &filename);

    bool ImportJson(const std::string &filename);
};
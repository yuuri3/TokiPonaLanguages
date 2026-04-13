#pragma once

#include "LanguageDifference.h"
#include "PhonemeTable.h"
#include "GeometryTable.h"
#include "PhonologicalChange.h"
#include "GeometryDifference.h"
#include "PeriodDifference.h"

class Language;
struct TableData;

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
    const GeometryTable &GetGeometryTable() const;
    const PhonemeTable &GetPhonemeTable() const;
    void AddDifference(const LanguageDifference &languageDifference);

    void EditGeometry(const std::vector<GeometryDifference> &differences);
    void EditPeriod(const std::vector<PeriodDifference> &differences);

    const bool Empty() const;
    std::optional<Language> CalculateLanguage(const int place, const int period);
    const TableData GetLanguageNames() const;

    const std::vector<PhonologicalChange> GetPhonologicalChanges(const int place, const int period);
    const std::vector<std::string> GetPhonologicalChangeStrings(const int place, const int period);
    bool SetPhonologicalChanges(const int place, const int period, const std::vector<PhonologicalChange> &phonologicalChange);
    bool SetPhonologicalChangesFromString(const int place, const int period, const std::vector<std::string> &phonologicalChange);

    std::vector<std::pair<int, int>> GetLoanwordIDs(const int targetPlace, const int referencePlace, const int period);
    bool SetLoanwords(const int targetPlace, const int referencePlace, const int period, const std::vector<std::pair<int, int>> &wordIDs);

    void Export(const std::string &filename);
    bool Import(const std::string &filename);

    bool ImportJson(const std::string &filename);

private:
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
};
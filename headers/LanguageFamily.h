#pragma once

#include "Utility.h"
#include "Random.h"
#include "Language.h"
#include "PhonologicalChange.h"
#include "LanguageDifferenceType.h"
#include <string>

/**
 * @brief 語族差分
 *
 */
struct LanguageDifference
{
    // タイプ
    LanguageDifferenceType Type;
    // 時代
    int Period;
    // 整数パラメータ
    std::vector<int> IntParam;
    // 実数パラメータ
    std::vector<double> DoubleParam;
    // 文字列パラメータ
    std::vector<std::string> StringParam;
    // 音韻変化（あとで消す）
    PhonologicalChange PhonologicalChanges;
    // 意味変化（あとで消す）
    Meaning SemanticChange;

    static LanguageDifference CreateAddWord(const std::string &ID, const int period, const int wordID, const std::string &wordForm);
    static LanguageDifference CreateChangeStrength(const std::string &ID, const int period, const double strength);
    static LanguageDifference CreatePhonologicalChange(const std::string &ID, const int period, const int wordID, const PhonologicalChange phonologicalChange);
    static LanguageDifference CreateSemanticChange(const std::string &ID, const int period, const int wordID, const Meaning meaning);
    static LanguageDifference CreateLoanword(const std::string &ID1, const std::string &ID2, const int period, const int wordID1, const int wordID2);
    static LanguageDifference CreateAddCompound(const std::string &ID, const int period, const int wordID, const std::vector<int> wordIDs);
    static LanguageDifference CreateObsoleteWord(const std::string &ID, const int period, const int wordID);
};

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhonemeConverter
{
    std::map<std::string, Phomene> Map;
    PhonemeConverter static Create(const std::vector<std::vector<std::string>> &table);

    std::vector<Phomene> ConvertToPhoneme(const std::string &str);
    Language convertToLanguage(const std::vector<std::string> &strs);
};

/**
 * @brief 語族
 *
 */
struct LanguageFamily
{
    // 時代
    int Period = 0;
    // 地理
    std::vector<std::vector<std::string>> Geography;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable;
    // 地理と言語の対応（シミュレーション用）
    std::map<std::string, Language> Languages;
    // 祖語（シミュレーション用）
    Language ProtoLanguage;
    // 祖語からの差分
    std::vector<LanguageDifference> languageDifference;

    void SetProtoLanguageOnGeography(
        const std::string &startPlace,
        const Language &protoLanguage);
    std::vector<std::string> GetWords(std::string place);
    void PhonologicalChangeRandom(
        const double pPhonologicalChange,
        const double pSoundLoss,
        const bool isProhibitMinimalPair = true,
        const bool isSoundDuplication = true);
    void SemanticChangeRandom(
        const double pSemanticShift,
        const double maxSemanticShiftRate);
    void LoanwordRandom(const int nLoanword, const double pLoanword);
    void ChangeLanguageStrengthRandom(const double pChangeStrength);
    void ObsoleteWordRandom(const double pWordLoss);
    void MakeCompoundRandom(const double pWordBirth);
    void ExportLanguageToCSV(const std::string &filename);
    bool HasAllPlaceLanguage();
    void ToNextPeriod();
    void ApplyDifference(const LanguageDifference &diff);
    void ApplyDifferences(const std::vector<LanguageDifference> &diffs);
    void Export(const std::string &filename);
    void Import(const std::string &filename);
};

std::string convertToString(const std::vector<Phomene> &Phonemes, const std::vector<std::vector<std::string>> &table);
PhonologicalChange makepPhonologicalChangeRandom(const Phomene &beforePhoneme, const std::vector<std::vector<std::string>> &table, const double pRemovePhoneme);
Phomene getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table);
Phomene getRandomSoundFromLanguage(Language &language);
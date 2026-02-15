#pragma once

#include "Utility.h"
#include "Random.h"
#include "PhonemeConverter.h"
#include "LanguageDifference.h"

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
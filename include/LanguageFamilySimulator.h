#pragma once

#include "Utility.h"
#include "Random.h"
#include "PhonemeConverter.h"
#include "LanguageDifference.h"
#include "LanguageFamily.h"
#include "stdafx.h"

/**
 * @brief 語族の時間発展をシミュレート
 *
 */
struct LanguageFamilySimulator
{
    // 語族
    LanguageFamily LanguageFamily_;
    // 時代
    int Period = 0;
    // 地理と言語の対応（シミュレーション用）
    std::map<std::string, Language> Languages;
    // 祖語（シミュレーション用）
    Language ProtoLanguage;

    static std::optional<LanguageFamilySimulator> Create();
    static std::optional<LanguageFamilySimulator> Create(LanguageFamily languageFamily);
    void SetProtoLanguageOnGeography(
        const std::string &startPlace,
        const Language &protoLanguage);
    std::vector<std::string> GetWords(std::string place);
    void PhonologicalChangeRandom(
        const double pPhonologicalChange,
        const double pSoundLoss,
        const bool isProhibitMinimalPair = true,
        const bool isSoundDuplication = true);
    void LoanwordRandom(const int nLoanword, const double pLoanword);
    void ChangeLanguageStrengthRandom(const double pChangeStrength);
    bool HasAllPlaceLanguage();
    void ToNextPeriod();
    std::vector<std::vector<std::string>> ToStringLanguageFamily();
    std::vector<std::vector<std::string>> ToStringCurrentLanguages();
    std::optional<Language> CalculateLanguage(const std::string place, const int period);

private:
    bool ApplyDifference(const LanguageDifference &diff);
    bool ApplyDifferences(const std::vector<LanguageDifference> &diffs);
};

PhonologicalChange makepPhonologicalChangeRandom(const Phoneme &beforePhoneme, const std::vector<std::vector<std::string>> &table, const double pRemovePhoneme);
Phoneme getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table);
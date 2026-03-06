#pragma once

#include "LanguageFamily.h"
#include "Language.h"
#include "stdafx.h"

/**
 * @brief 語族の時間発展をシミュレート
 *
 */
struct LanguageFamilySimulator
{
private:
    // 語族
    LanguageFamily LanguageFamily_;
    // 時代
    int Period_ = 0;
    // 地理と言語の対応（シミュレーション用）
    std::map<std::string, Language> Languages_;

public:
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
    const LanguageFamily GetLanguages() const;
};
#pragma once

#include "Word.h"
#include "stdafx.h"
#include "LanguageDifference.h"

/**
 * @brief 言語
 *
 */
struct Language
{
private:
    // 影響度、大きい方から小さいほうへ単語が借用される
    double Strength_;
    void ApplyPhonologicalChange(const PhonologicalChange &phonologicalChange, const bool isProhibitSoundDuplication, const bool isProhibitMinimalPair);

public:
    // 語彙
    std::map<int, Word> Words_;

    void Reset();
    void Copy(const Language &lang);
    void ApplyDifference(const LanguageDifference &dif);
    double GetStrength() const;
    bool IsStronger(const Language &lang) const;
    LanguageDifference ChangeStrength(const std::string &place, const int period);
};
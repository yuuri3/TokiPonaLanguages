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
    // 語彙
    std::map<int, Word> Words_;

    void ApplyPhonologicalChange(const PhonologicalChange &phonologicalChange, const bool isProhibitSoundDuplication, const bool isProhibitMinimalPair);

public:
    void Reset();
    void Copy(const Language &lang);
    void ApplyDifference(const LanguageDifference &dif);
    void AddWord(const LanguageDifference &dif, const std::vector<Phoneme> &form);
    void AddWord(const std::vector<Phoneme> &form);
    void LoanWord(const LanguageDifference &dif, const Language &referenceLanguage);
    const double GetStrength() const;
    const bool IsStronger(const Language &lang) const;
    const LanguageDifference ChangeStrength(const std::string &place, const int period);
    const int CountWord() const;
    const std::pair<int, Word> GetNthWord(const int n) const;
    const std::optional<Word> GetWord(const int wordID) const;
    const bool Empty() const;
};
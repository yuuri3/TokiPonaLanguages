#pragma once

#include "Word.h"
#include "stdafx.h"

class PhonemeConverter;
class LanguageDifference;
class LanguageFamily;

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
    // 親語族のポインタ
    const LanguageFamily *Parent_;

    void ApplyPhonologicalChange(const PhonologicalChange &phonologicalChange, const bool isProhibitMinimalPair);

public:
    Language(const LanguageFamily *parent);
    ~Language();
    void Reset();
    void Copy(const Language &lang);
    void ApplyDifference(const LanguageDifference &dif);
    void AddWord(const LanguageDifference &dif, const std::vector<int> &form);
    void AddWord(const std::vector<int> &form);
    void LoanWord(const LanguageDifference &dif, const Language &referenceLanguage);
    const double GetStrength() const;
    const bool IsStronger(const Language &lang) const;
    const LanguageDifference ChangeStrength(const int &place, const int period);
    const int CountWord() const;
    const int GetNewWordID() const;
    const std::pair<const int, Word> &GetNthWord(const int n) const;
    const Word *GetWord(const int wordID) const;
    const bool Empty() const;
};

namespace LanguageUtility
{
    bool ApplyDifference(const LanguageDifference &diff, std::map<int, Language> &languages, const LanguageFamily *family);
    bool ApplyDifferences(const std::vector<LanguageDifference> &diffs, std::map<int, Language> &languages, const LanguageFamily *family);
}
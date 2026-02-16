#include "LanguageDifference.h"

/**
 * @brief Create a Add 単語 object
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param wordForm 語形
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddWord(const std::string &place, const int period, const int wordID, const std::string &wordForm)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::AddWord;
    diff.StringParam.emplace_back(place);
    diff.IntParam.emplace_back(wordID);
    diff.StringParam.emplace_back(wordForm);
    return diff;
}

/**
 * @brief Change 言語 影響度
 *
 * @param place 地域
 * @param period 時代
 * @param strength 影響度
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateChangeStrength(const std::string &place, const int period, const double strength)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::ChangeStrength;
    diff.StringParam.emplace_back(place);
    diff.DoubleParam.emplace_back(strength);
    return diff;
}

/**
 * @brief Change 言語 音韻
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param phonologicalChange 音韻変化
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreatePhonologicalChange(const std::string &place, const int period, const int wordID, const PhonologicalChange phonologicalChange)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::PhonologicalChange;
    diff.StringParam.emplace_back(place);
    diff.IntParam.emplace_back(wordID);
    diff.PhonologicalChanges = phonologicalChange;
    return diff;
}

/**
 * @brief Change 単語の意味
 *
 * @param place 地理
 * @param period 時代
 * @param wordID 単語ID
 * @param meaning 意味変化
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateSemanticChange(const std::string &place, const int period, const int wordID, const Meaning meaning)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::SemanticChange;
    diff.StringParam.emplace_back(place);
    diff.IntParam.emplace_back(wordID);
    diff.SemanticChange = meaning;
    return diff;
}

/**
 * @brief 借用
 *
 * @param place1 借用元言語地域
 * @param place2 借用先言語地域
 * @param period 時代
 * @param wordID1 借用元単語ID
 * @param wordID2 借用先単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateLoanword(const std::string &place1, const std::string &place2, const int period, const int wordID1, const int wordID2)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::Loanword;
    diff.StringParam.emplace_back(place1);
    diff.IntParam.emplace_back(wordID1);
    diff.StringParam.emplace_back(place2);
    diff.IntParam.emplace_back(wordID2);
    return diff;
}

/**
 * @brief 複合語
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param wordIDs 参照単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddCompound(const std::string &place, const int period, const int wordID, const std::vector<int> wordIDs)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::AddCompound;
    diff.StringParam.emplace_back(place);
    diff.IntParam.emplace_back(wordID);
    diff.IntParam.insert(diff.IntParam.end(), wordIDs.begin(), wordIDs.end());
    return diff;
}

/**
 * @brief 単語削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateObsoleteWord(const std::string &place, const int period, const int wordID)
{
    LanguageDifference diff;
    diff.Period = period;
    diff.Type = LanguageDifferenceType::ObsoleteWord;
    diff.StringParam.emplace_back(place);
    diff.IntParam.emplace_back(wordID);
    return diff;
}
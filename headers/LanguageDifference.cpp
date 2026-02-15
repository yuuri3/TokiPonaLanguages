#include "LanguageDifference.h"

/**
 * @brief Create a Add 単語 object
 *
 * @param ID 言語ID
 * @param period 時代
 * @param wordID 単語ID
 * @param wordForm 語形
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddWord(const std::string &ID, const int period, const int wordID, const std::string &wordForm)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::AddWord;
    result.StringParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.StringParam.emplace_back(wordForm);
    return result;
}

/**
 * @brief Change 言語 影響度
 *
 * @param ID 言語ID
 * @param period 時代
 * @param strength 影響度
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateChangeStrength(const std::string &ID, const int period, const double strength)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::ChangeStrength;
    result.StringParam.emplace_back(ID);
    result.DoubleParam.emplace_back(strength);
    return result;
}

/**
 * @brief Change 言語 音韻
 *
 * @param ID 言語ID
 * @param period 時代
 * @param wordID 単語ID
 * @param phonologicalChange 音韻変化
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreatePhonologicalChange(const std::string &ID, const int period, const int wordID, const PhonologicalChange phonologicalChange)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::PhonologicalChange;
    result.StringParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.PhonologicalChanges = phonologicalChange;
    return result;
}

/**
 * @brief Change 単語の意味
 *
 * @param ID 言語ID
 * @param period 時代
 * @param wordID 単語ID
 * @param meaning 意味変化
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateSemanticChange(const std::string &ID, const int period, const int wordID, const Meaning meaning)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::SemanticChange;
    result.StringParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.SemanticChange = meaning;
    return result;
}

/**
 * @brief 借用
 *
 * @param ID1 借用元言語ID
 * @param ID2 借用先言語ID
 * @param period 時代
 * @param wordID1 借用元単語ID
 * @param wordID2 借用先単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateLoanword(const std::string &ID1, const std::string &ID2, const int period, const int wordID1, const int wordID2)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::Loanword;
    result.StringParam.emplace_back(ID1);
    result.IntParam.emplace_back(wordID1);
    result.StringParam.emplace_back(ID2);
    result.IntParam.emplace_back(wordID2);
    return result;
}

/**
 * @brief 複合語
 *
 * @param ID 言語ID
 * @param period 時代
 * @param wordID 単語ID
 * @param wordIDs 参照単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddCompound(const std::string &ID, const int period, const int wordID, const std::vector<int> wordIDs)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::AddCompound;
    result.StringParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.IntParam.insert(result.IntParam.end(), wordIDs.begin(), wordIDs.end());
    return result;
}

/**
 * @brief 単語削除
 *
 * @param ID 言語ID
 * @param period 時代
 * @param wordID 単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateObsoleteWord(const std::string &ID, const int period, const int wordID)
{
    LanguageDifference result;
    result.Period = period;
    result.Type = LanguageDifferenceType::ObsoleteWord;
    result.StringParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    return result;
}
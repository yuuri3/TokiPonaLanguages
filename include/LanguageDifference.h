#pragma once

#include "LanguageDifferenceType.h"
#include "PhonologicalChange.h"
#include "stdafx.h"

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

    static LanguageDifference CreateAddWord(const std::string &place, const int period, const int wordID, const std::string &wordForm);
    static LanguageDifference CreateChangeStrength(const std::string &place, const int period, const double strength);
    static LanguageDifference CreatePhonologicalChange(const std::string &place, const int period, const int wordID, const PhonologicalChange phonologicalChange);
    static LanguageDifference CreateLoanword(const std::string &place1, const std::string &place2, const int period, const int wordID1, const int wordID2);
    static LanguageDifference CreateAddCompound(const std::string &place, const int period, const int wordID, const std::vector<int> wordIDs);
    static LanguageDifference CreateObsoleteWord(const std::string &place, const int period, const int wordID);
};
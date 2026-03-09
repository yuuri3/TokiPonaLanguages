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
private:
    // タイプ
    LanguageDifferenceType Type_;
    // 時代
    int Period_;
    // 整数パラメータ
    std::vector<int> IntParam_;
    // 実数パラメータ
    std::vector<double> DoubleParam_;
    // 文字列パラメータ
    std::vector<std::string> StringParam_;
    // 音韻変化（あとで消す）
    PhonologicalChange PhonologicalChanges_;

public:
    static LanguageDifference CreateAddWord(const std::string &place, const int period, const int wordID, const std::string &wordForm);
    static LanguageDifference CreateChangeStrength(const std::string &place, const int period, const double strength);
    static LanguageDifference CreatePhonologicalChange(const std::string &place, const int period, const PhonologicalChange phonologicalChange);
    static LanguageDifference CreateLoanword(const std::string &place1, const std::string &place2, const int period, const int wordID1, const int wordID2);
    static LanguageDifference CreateAddCompound(const std::string &place, const int period, const int wordID, const std::vector<int> wordIDs);
    static LanguageDifference CreateObsoleteWord(const std::string &place, const int period, const int wordID);
    static LanguageDifference CreateEditPart(const std::string &place, const int period, const int wordID, const int partID, const std::string &part);
    static LanguageDifference CreateDeletePart(const std::string &place, const int period, const int wordID, const int partID);
    static LanguageDifference CreateEditTranslation(const std::string &place, const int period, const int wordID, const int partID, const int translationID, const std::string &translation);
    static LanguageDifference CreateDeleteTranslation(const std::string &place, const int period, const int wordID, const int partID, const int translationID);
    static LanguageDifference CreateEditTag(const std::string &place, const int period, const int wordID, const int tagID, const std::string &tag);

    const LanguageDifferenceType &GetType() const;
    const int GetPeriod() const;
    void AddPeriod();
    void SubPeriod();

    const std::optional<int> IntParam(const int i) const;
    const int IntParamSize() const;
    const std::optional<double> DoubleParam(const int i) const;
    const int DoubleParamSize() const;
    const std::optional<std::string> StringParam(const int i) const;
    const int StringParamSize() const;
    const PhonologicalChange &GetPhonologicalChange() const;

    static bool Import(std::ifstream &file, LanguageDifference &dif);
    void Export(std::ofstream &file) const;
};
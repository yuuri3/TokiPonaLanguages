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
    // 地理
    int Place_;
    // 整数パラメータ
    std::vector<int> IntParam_;
    // 実数パラメータ
    std::vector<double> DoubleParam_;
    // 文字列パラメータ
    std::vector<std::string> StringParam_;
    // 音素パラメータ
    std::vector<int> PhonemeIDs_;
    // 音韻変化（あとで消す）
    PhonologicalChange PhonologicalChanges_;

public:
    static LanguageDifference CreateAddWord(const int place, const int period, const int wordID, const std::vector<int> &wordForm);
    static LanguageDifference CreateChangeStrength(const int place, const int period, const double strength);
    static LanguageDifference CreatePhonologicalChange(const int place, const int period, const PhonologicalChange phonologicalChange);
    static LanguageDifference CreateLoanword(const int place1, const int place2, const int period, const int wordID1, const int wordID2);
    static LanguageDifference CreateAddCompound(const int place, const int period, const int wordID, const std::vector<int> wordIDs);
    static LanguageDifference CreateObsoleteWord(const int place, const int period, const int wordID);
    static LanguageDifference CreateEditPart(const int place, const int period, const int wordID, const int partID, const std::string &part);
    static LanguageDifference CreateDeletePart(const int place, const int period, const int wordID, const int partID);
    static LanguageDifference CreateEditTranslation(const int place, const int period, const int wordID, const int partID, const int translationID, const std::string &translation);
    static LanguageDifference CreateDeleteTranslation(const int place, const int period, const int wordID, const int partID, const int translationID);
    static LanguageDifference CreateEditTag(const int place, const int period, const int wordID, const int tagID, const std::string &tag);
    static LanguageDifference CreateDeleteTag(const int place, const int period, const int wordID, const int tagID);
    static LanguageDifference CreateEditContent(const int place, const int period, const int wordID, const int contentID, const std::string &title, const std::string &content);
    static LanguageDifference CreateDeleteContent(const int place, const int period, const int wordID, const int contentID);
    static LanguageDifference CreateEditVariation(const int place, const int period, const int wordID, const int variationID, const std::string &title, const std::vector<int> variation);
    static LanguageDifference CreateDeleteVariation(const int place, const int period, const int wordID, const int variationID);
    static LanguageDifference CreateSetRelation(const int place, const int period, const int wordID, const int relationID, const std::string &title, const int targetWordID);
    static LanguageDifference CreateDeleteRelation(const int place, const int period, const int wordID, const int relationID);

    const LanguageDifferenceType &GetType() const;
    const int GetPeriod() const;
    const int GetPlace() const;
    void AddPeriod();
    void SubPeriod();

    const std::optional<int> IntParam(const int i) const;
    const int IntParamSize() const;
    const std::optional<double> DoubleParam(const int i) const;
    const int DoubleParamSize() const;
    const std::optional<std::string> StringParam(const int i) const;
    const int StringParamSize() const;
    const std::vector<int> &GetPhonemeParam() const;
    const PhonologicalChange &GetPhonologicalChange() const;

    static bool Import(std::ifstream &file, LanguageDifference &dif);
    void Export(std::ofstream &file) const;
};
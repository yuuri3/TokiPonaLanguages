#pragma once

#include "stdafx.h"

class Language;
class PhonologicalChange;
class PhonemeTable;

/**
 * @brief 単語
 *
 */
struct Word
{
private:
    // entry
    //   ID
    int ID;
    //   form
    std::vector<int> Form_;
    // translation <title,form>
    std::map<int, std::pair<std::string, std::map<int, std::string>>> Translations_;
    // tags
    std::map<int, std::string> Tags_;
    // contents <title,text>
    std::map<int, std::pair<std::string, std::string>> Contents_;
    // variations <title,form>
    std::map<int, std::pair<std::string, std::vector<int>>> Variations_;
    // relations <title,entry>
    std::map<int, std::pair<std::string, int>> Relations_;

public:
    bool operator==(const Word &other) const
    {
        return Form_ == other.Form_;
    }

    bool operator!=(const Word &other) const
    {
        return !operator==(other);
    }

    bool operator<(const Word &other) const
    {
        return Form_ < other.Form_;
    }

    static Word Create(const std::vector<int> &form);
    static Word CreateFromJsonObject(const QJsonObject &obj, const PhonemeTable &table);
    Word Add(const Word &word) const;

    const std::vector<int> GetForm() const;
    const std::vector<int> GetPartIDs() const;
    const std::vector<int> GetTranslationIDs(const int partID) const;
    const std::string GetPart(const int partID) const;
    const std::string GetTranslation(const int partID, const int translationID) const;
    void SetPart(const int partID, const std::string &part);
    void DeletePart(const int partID);
    void SetTranslation(const int partID, const int translationID, const std::string &translation);
    void DeleteTranslation(const int partID, const int translationID);
    const std::vector<int> GetTagIDs() const;
    const std::string GetTag(const int tagID) const;
    void SetTag(const int tagID, const std::string &tag);
    void DeleteTag(const int tagID);
    const std::vector<int> GetContentIDs() const;
    const std::string GetContentTitle(const int contentID) const;
    const std::string GetContent(const int contentID) const;
    void SetContent(const int contentID, const std::string &title, const std::string &content);
    void DeleteContent(const int contentID);
    const std::vector<int> GetVariationIDs() const;
    const std::string GetVariationTitle(const int contentID) const;
    const std::vector<int> GetVariation(const int contentID) const;
    void SetVariation(const int cariationID, const std::string &title, const std::vector<int> &content);
    void DeleteVariation(const int variationID);
    const std::vector<int> GetRelationIDs() const;
    const std::string GetRelationTitle(const int relationID) const;
    const int GetRelationWordID(const int relationID) const;
    void SetRelation(const int relationID, const std::string &title, const int targetWordID);
    void DeleteRelation(const int relationID);

    const std::vector<std::string> GetAllTranslations() const;
    void ChangeSound(PhonologicalChange phon, const PhonemeTable &phonemeTable);
};
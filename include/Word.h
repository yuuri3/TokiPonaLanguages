#pragma once

#include "Phoneme.h"
#include "stdafx.h"

class Language;
class PhonologicalChange;
class PhonemeConverter;

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
    std::vector<Phoneme> Form_;
    // translation <title,form>
    std::map<int, std::pair<std::string, std::map<int, std::string>>> Translations_;
    // tags
    std::vector<std::string> Tags_;
    // contents <title,text>
    std::map<std::string, std::string> Contents_;
    // variations <title,form>
    std::map<std::string, std::vector<Phoneme>> Variations_;
    // relations <title,entry>
    std::map<std::string, int> Relations_;

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

    static Word Create(const std::vector<Phoneme> &form);
    static Word CreateFromJsonObject(const QJsonObject &obj, const PhonemeConverter &converter);
    Word Add(const Word &word) const;

    const std::vector<Phoneme> GetForm() const;
    const std::map<std::string, std::vector<std::string>> GetTranslations() const;
    void SetPart(const int partID, const std::string &part);
    void DeletePart(const int partID);
    void SetTranslation(const int partID, const int translationID, const std::string &translation);
    void DeleteTranslation(const int partID, const int translationID);
    const std::vector<std::string> GetTags() const;
    const std::map<std::string, std::string> GetContents() const;
    const std::map<std::string, std::vector<Phoneme>> GetVariations() const;
    const std::map<std::string, int> GetRealtions() const;
    const std::vector<std::string> GetAllTranslations() const;
    void ChangeSound(PhonologicalChange phon, const bool isProhibitSoundDuplication);
};
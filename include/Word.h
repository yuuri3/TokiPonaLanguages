#pragma once

#include "Phoneme.h"
#include "stdafx.h"

class Language;

/**
 * @brief 単語
 *
 */
struct Word
{
    // entry
    //   ID
    int ID;
    //   form
    std::vector<Phoneme> Form_;
    // translation <title,form>
    std::map<std::string, std::vector<std::string>> Translations_;
    // tags
    std::vector<std::string> Tags_;
    // contents <title,text>
    std::map<std::string, std::string> Contents_;
    // variations <title,form>
    std::map<std::string, std::vector<Phoneme>> Variations_;
    // relations <title,entry>
    std::map<std::string, int> Relations_;

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

    Word Add(const Word &word) const;
};
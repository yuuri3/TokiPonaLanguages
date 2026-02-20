#pragma once

#include "Phoneme.h"
#include <vector>
#include <map>
#include <string>

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
    std::vector<Phomene> Form;
    // translation <title,form>
    std::map<std::string, std::vector<std::string>> Translations;
    // tags
    std::vector<std::string> Tags;
    // contents <title,text>
    std::map<std::string, std::string> Contents;
    // variations <title,form>
    std::map<std::string, std::vector<Phomene>> Variations;
    // relations <title,entry>
    std::map<std::string, int> Relations;

    bool operator==(const Word &other) const
    {
        return Form == other.Form;
    }

    bool operator!=(const Word &other) const
    {
        return !operator==(other);
    }

    bool operator<(const Word &other) const
    {
        return Form < other.Form;
    }

    Word Add(const Word &word) const;
};
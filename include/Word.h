#pragma once

#include "Phoneme.h"
#include <vector>

class Language;

/**
 * @brief 単語
 *
 */
struct Word
{
    // 発音
    std::vector<Phomene> Form;

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
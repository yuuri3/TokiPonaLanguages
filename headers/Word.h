#pragma once

#include "Phoneme.h"
#include "Meaning.h"
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
    // 意味
    Meaning Meanings;
    // 意味が対応する祖語の単語
    std::vector<Phomene> ReconstructedWord;

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
    void UpdateReconstructedWord(const Language &protoLanguage);
};
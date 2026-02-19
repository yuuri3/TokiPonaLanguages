#pragma once

#include "Word.h"
#include <vector>

class Language;

/**
 * @brief シミュレーション用に情報を追加した単語
 *
 */
struct WordForSimulation
{
    // 単語
    Word Word_;
    // 意味
    Meaning Meanings;
    // 意味が対応する祖語の単語
    std::vector<Phomene> ReconstructedWord;

    bool operator==(const WordForSimulation &other) const
    {
        return Word_ == other.Word_;
    }

    bool operator!=(const WordForSimulation &other) const
    {
        return !operator==(other);
    }

    bool operator<(const WordForSimulation &other) const
    {
        return Word_ < other.Word_;
    }

    WordForSimulation Add(const WordForSimulation &word) const;
    void UpdateReconstructedWord(const Language &protoLanguage);
    std::vector<Phomene> &GetForm();
    std::vector<Phomene> GetForm() const;
};
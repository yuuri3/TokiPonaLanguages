#pragma once

#include "Word.h"
#include <vector>

class Language;

/**
 * @brief シミュレーション用に情報を追加した単語
 *
 */
struct WordForSimulation : public Word
{
    // 意味
    Meaning Meanings;
    // 意味が対応する祖語の単語
    std::vector<Phomene> ReconstructedWord;

    WordForSimulation Add(const WordForSimulation &word) const;
    void UpdateReconstructedWord(const Language &protoLanguage);
};
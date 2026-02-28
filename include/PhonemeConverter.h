#pragma once

#include "Language.h"
#include "stdafx.h"

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhonemeConverter
{
    std::map<std::string, Phoneme> PhonemeMap_;
    PhonemeConverter static Create(const std::vector<std::vector<std::string>> &phonemeTable);

    std::vector<Phoneme> ConvertToPhoneme(const std::string &str) const;
    std::string ConvertToString(const std::vector<Phoneme> &Phonemes) const;
    Language convertToLanguage(const std::vector<std::string> &strs) const;
    Phoneme GetRandom() const;
};
#pragma once

#include "Language.h"

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhonemeConverter
{
    std::map<std::string, Phomene> PhonemeMap;
    PhonemeConverter static Create(const std::vector<std::vector<std::string>> &phonemeTable);

    std::vector<Phomene> ConvertToPhoneme(const std::string &str);
    std::string ConvertToString(const std::vector<Phomene> &Phonemes);
    Language convertToLanguage(const std::vector<std::string> &strs);
};
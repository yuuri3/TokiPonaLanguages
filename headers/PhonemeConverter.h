#pragma once

#include "Language.h"

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhonemeConverter
{
    std::map<std::string, Phomene> Map;
    PhonemeConverter static Create(const std::vector<std::vector<std::string>> &table);

    std::vector<Phomene> ConvertToPhoneme(const std::string &str);
    Language convertToLanguage(const std::vector<std::string> &strs);
};
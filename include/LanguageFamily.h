#pragma once

#include "Utility.h"
#include "Random.h"
#include "PhonemeConverter.h"
#include "LanguageDifference.h"

/**
 * @brief 語族
 *
 */
struct LanguageFamily
{
    // 地理
    std::vector<std::vector<std::string>> Geography;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable;
    // 差分
    std::vector<LanguageDifference> languageDifference;

    void Export(const std::string &filename);
    void Import(const std::string &filename);
};
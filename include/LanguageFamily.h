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
    std::vector<std::vector<std::string>> Geography_;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable_;
    // 差分
    std::vector<LanguageDifference> languageDifference_;

    void Export(const std::string &filename);
    bool Import(const std::string &filename);
};
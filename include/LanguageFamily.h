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
private:
    // 地理
    std::vector<std::vector<std::string>> Geography_;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable_;

public:
    // 差分
    std::vector<LanguageDifference> languageDifference_;

    static LanguageFamily Create(const std::vector<std::vector<std::string>> &geography, const std::vector<std::vector<std::string>> &phonemeTable);
    const std::vector<std::vector<std::string>> GetGeography() const;
    const std::vector<std::vector<std::string>> GetPhonemeTable() const;

    void Export(const std::string &filename);
    bool Import(const std::string &filename);
};
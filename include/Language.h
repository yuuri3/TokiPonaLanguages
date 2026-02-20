#pragma once

#include "Word.h"
#include "stdafx.h"

/**
 * @brief 言語
 *
 */
struct Language
{
    // 言語名
    std::vector<Phomene> Name;
    // 影響度、大きい方から小さいほうへ単語が借用される
    double Strength;
    // 語彙
    std::map<int, Word> Words;
};
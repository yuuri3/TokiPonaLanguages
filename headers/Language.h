#pragma once

#include "WordForSimulation.h"

/**
 * @brief 言語
 *
 */
struct Language
{
    // 影響度、大きい方から小さいほうへ単語が借用される
    double Strength;
    // 語彙
    std::map<int, WordForSimulation> Words;
};
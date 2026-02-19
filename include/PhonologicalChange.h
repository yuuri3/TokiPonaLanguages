#pragma once

#include "Phoneme.h"
#include "PhoneticEnvironment.h"

/**
 * @brief 音韻変化
 *
 */
struct PhonologicalChange
{
    // 変化前の音韻
    Phomene BeforePhoneme;
    // 条件
    PhoneticEnvironment PhoneticEnvironment;
    // 音韻が消えるか
    bool IsRemove;
    // 変化前の音韻
    Phomene AfterPhoneme;
};
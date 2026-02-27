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
    Phoneme BeforePhoneme_;
    // 条件
    PhoneticEnvironment PhoneticEnvironment_;
    // 音韻が消えるか
    bool IsRemove_;
    // 変化前の音韻
    Phoneme AfterPhoneme_;
};
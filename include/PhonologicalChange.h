#pragma once

#include "PhoneticItem.h"

/**
 * @brief 音韻変化
 *
 */
struct PhonologicalChange
{
    // 変化前の音韻
    std::vector<PhoneticItem> BeforePhoneticItems_;
    // 変化前の音韻
    std::vector<PhoneticItem> AfterPhoneticItems_;
    // 条件
    std::vector<std::pair<int, PhoneticItem>> PhoneticEnvironment_;
};
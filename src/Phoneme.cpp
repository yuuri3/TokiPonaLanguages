#include "Phoneme.h"

/**
 * @brief 空白か
 *
 * @return true
 * @return false
 */
const bool Phoneme::IsSpace() const
{
    return IsSpace_;
}

/**
 * @brief 空白を生成
 *
 * @return Phoneme
 */
Phoneme Phoneme::MakeSpace()
{
    Phoneme space;
    space.IsSpace_ = true;
    space.Manner_ = -1;
    space.Place_ = -1;
    return space;
}
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

/**
 * @brief インスタンスを生成
 *
 * @param place 調音部位
 * @param manner 調音方法
 * @return Phoneme
 */
Phoneme Phoneme::Create(const int place, const int manner)
{
    Phoneme result;
    result.IsSpace_ = false;
    result.Place_ = place;
    result.Manner_ = manner;
    return result;
}

/**
 * @brief 子音か
 *
 * @return true
 * @return false
 */
const bool Phoneme::IsConsonant() const
{
    return Manner_ <= MAX_CONSONANT_MANNER;
}

/**
 * @brief 母音か
 *
 * @return true
 * @return false
 */
const bool Phoneme::IsVowel() const
{
    return Manner_ > MAX_CONSONANT_MANNER;
}

/**
 * @brief 調音部位をゲット
 *
 * @return const int
 */
const int Phoneme::GetPlace() const
{
    return Place_;
}

/**
 * @brief 調音方法をゲット
 *
 * @return const int
 */
const int Phoneme::GetManner() const
{
    return Manner_;
}
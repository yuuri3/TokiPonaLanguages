#include "Meaning.h"
#include <cmath>

namespace
{
    double TOLERANCE = 1.0e-6;
}

/**
 * @brief 意味ベクトルの加算
 *
 * @param meaning 可算する意味ベクトル
 */
Meaning Meaning::Add(const Meaning &meaning) const
{
    Meaning result = *this;
    for (const auto &[key, value] : meaning)
    {
        result[key] += value;
    }
    return result;
}

/**
 * @brief 意味ベクトルの内積
 *
 * @param meaning 掛ける意味ベクトル
 */
double Meaning::Dot(const Meaning &meaning) const
{
    double result = 0.0;

    for (const auto &[key, value] : meaning)
    {
        auto it = find(key);
        if (it != end())
        {
            result += value * it->second;
        }
    }

    return result;
}

/**
 * @brief 実数倍
 *
 * @param scalar 掛ける実数
 */
Meaning Meaning::Product(const double scalar) const
{
    Meaning result = *this;
    for (auto &[key, value] : result)
    {
        value *= scalar;
    }
    return result;
}

/**
 * @brief 正規化
 *
 */
void Meaning::Normalize()
{
    const double square = Dot(*this);
    if (square <= TOLERANCE * TOLERANCE) // sqrtの前に判定
    {
        return;
    }
    const double inverseNorm = 1.0 / std::sqrt(square);
    for (auto &[key, value] : *this)
    {
        value *= inverseNorm;
    }
}
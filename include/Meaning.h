#pragma once

#include <map>
#include <string>

/**
 * @brief 意味ベクトル
 *
 */
class Meaning : public std::map<std::string, double>
{
public:
    Meaning Add(const Meaning &meaning) const;
    double Dot(const Meaning &meaning) const;
    Meaning Product(const double scalar) const;
    void Normalize();
};
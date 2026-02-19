#pragma once

/**
 * @brief 音韻
 *
 */
struct Phomene
{
    // 調音方法
    int Manner;
    // 調音部位
    int Place;

    bool operator==(const Phomene &other) const
    {
        return (Manner == other.Manner && Place == other.Place);
    }

    bool operator!=(const Phomene &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Phomene &other) const
    {
        if (Manner != other.Manner)
            return Manner < other.Manner;
        return Place < other.Place;
    }
};
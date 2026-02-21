#pragma once

/**
 * @brief 音韻
 *
 */
struct Phoneme
{
    // 調音方法
    int Manner;
    // 調音部位
    int Place;

    bool operator==(const Phoneme &other) const
    {
        return (Manner == other.Manner && Place == other.Place);
    }

    bool operator!=(const Phoneme &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Phoneme &other) const
    {
        if (Manner != other.Manner)
            return Manner < other.Manner;
        return Place < other.Place;
    }
};
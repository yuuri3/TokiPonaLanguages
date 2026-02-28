#pragma once

/**
 * @brief 音韻
 *
 */
struct Phoneme
{
private:
    // 無音フラグ
    bool IsSpace_ = false;

public:
    // 調音方法
    int Manner_;
    // 調音部位
    int Place_;

    bool operator==(const Phoneme &other) const
    {
        return (Manner_ == other.Manner_ && Place_ == other.Place_);
    }

    bool operator!=(const Phoneme &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Phoneme &other) const
    {
        if (Manner_ != other.Manner_)
            return Manner_ < other.Manner_;
        return Place_ < other.Place_;
    }

    const bool IsSpace() const;
    static Phoneme MakeSpace();
};
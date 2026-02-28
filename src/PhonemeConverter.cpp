#include "PhonemeConverter.h"
#include "Random.h"

PhonemeConverter PhonemeConverter::Create(const std::vector<std::vector<std::string>> &phonemeTable)
{
    PhonemeConverter converter;

    for (int row = 0; row < (int)phonemeTable.size(); ++row)
    {
        for (int column = 0; column < (int)phonemeTable[row].size(); ++column)
        {
            const std::string &item = phonemeTable[row][column];
            Phoneme phoneme;
            phoneme.Manner_ = row;
            phoneme.Place_ = column;
            converter.PhonemeMap_[item] = phoneme;
        }
    }
    return converter;
}

/**
 * 文字列を変換表に基づいて音素列に変換する
 * @param str 文字列
 */
std::vector<Phoneme> PhonemeConverter::ConvertToPhoneme(const std::string &str) const
{
    std::vector<Phoneme> convertedPhoneme;
    convertedPhoneme.reserve(str.length());

    for (size_t charPosition = 0; charPosition < str.length();)
    {
        bool isMatch = false;
        const int phonemeCharCount = 1;
        if (charPosition + phonemeCharCount <= str.length())
        {
            std::string PhonemeStr = str.substr(charPosition, phonemeCharCount);
            auto it = PhonemeMap_.find(PhonemeStr);
            if (it != PhonemeMap_.end())
            {
                convertedPhoneme.push_back(it->second);
                charPosition += phonemeCharCount;
                isMatch = true;
            }
            if (PhonemeStr == " ")
            {
                Phoneme space;
                space.IsSpace_ = true;
                space.Manner_ = -1;
                space.Place_ = -1;
                convertedPhoneme.emplace_back(space);
                charPosition += phonemeCharCount;
                isMatch = true;
            }
        }
        if (!isMatch)
            charPosition++;
    }
    return convertedPhoneme;
}

/**
 * @brief 文字列の配列を言語に変換する
 * @param strs 文字列の配列
 * @return 言語
 */
Language PhonemeConverter::convertToLanguage(const std::vector<std::string> &strs) const
{
    Language convertedLanguage;
    convertedLanguage.Reset();

    for (const auto &str : strs)
    {
        convertedLanguage.AddWord(ConvertToPhoneme(str));
    }
    return convertedLanguage;
}

/**
 * 音素列を変換表に基づいて文字列に復元する
 * @param Phonemes 音素列
 * @param table 音素表
 */
std::string PhonemeConverter::ConvertToString(const std::vector<Phoneme> &phonemes) const
{
    std::string str = "";

    for (const auto &phoneme : phonemes)
    {
        for (const auto &[keyString, valuePhoneme] : PhonemeMap_)
        {
            if (phoneme == valuePhoneme)
            {
                str += keyString;
            }
        }
        if (phoneme.IsSpace_)
        {
            str += " ";
        }
    }

    return str;
}

Phoneme PhonemeConverter::GetRandom() const
{
    const int randomIndex = getRandomInt(0, static_cast<int>(PhonemeMap_.size()));
    auto it = PhonemeMap_.begin();
    std::advance(it, randomIndex);
    return it->second;
}
#include "PhonemeConverter.h"

PhonemeConverter PhonemeConverter::Create(const std::vector<std::vector<std::string>> &phonemeTable)
{
    PhonemeConverter converter;

    for (int row = 0; row < (int)phonemeTable.size(); ++row)
    {
        for (int column = 0; column < (int)phonemeTable[row].size(); ++column)
        {
            const std::string &item = phonemeTable[row][column];
            Phomene phoneme;
            phoneme.Manner = row;
            phoneme.Place = column;
            converter.PhonemeMap[item] = phoneme;
        }
    }
    return converter;
}

/**
 * 文字列を変換表に基づいて音素列に変換する
 * @param str 文字列
 */
std::vector<Phomene> PhonemeConverter::ConvertToPhoneme(const std::string &str)
{
    std::vector<Phomene> convertedPhoneme;
    convertedPhoneme.reserve(str.length());

    for (size_t charPosition = 0; charPosition < str.length();)
    {
        bool isMatch = false;
        // 最長一致を優先（最大長が既知ならその値から開始）
        for (size_t phonemeCharCount = 2; phonemeCharCount > 0; --phonemeCharCount)
        {
            if (charPosition + phonemeCharCount <= str.length())
            {
                std::string PhonemeStr = str.substr(charPosition, phonemeCharCount);
                auto it = PhonemeMap.find(PhonemeStr);
                if (it != PhonemeMap.end())
                {
                    convertedPhoneme.push_back(it->second);
                    charPosition += phonemeCharCount;
                    isMatch = true;
                    break;
                }
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
Language PhonemeConverter::convertToLanguage(const std::vector<std::string> &strs)
{
    Language convertedLanguage;
    int wordID = 0;
    for (const auto &str : strs)
    {
        WordForSimulation word;
        word.GetForm() = ConvertToPhoneme(str);
        word.Meanings[str] = 1.0;
        word.ReconstructedWord = word.GetForm();
        convertedLanguage.Words[wordID] = word;
        wordID++;
    }
    convertedLanguage.Strength = 0.0;
    return convertedLanguage;
}
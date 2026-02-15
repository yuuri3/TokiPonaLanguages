#include "PhonemeConverter.h"

PhonemeConverter PhonemeConverter::Create(const std::vector<std::vector<std::string>> &table)
{
    PhonemeConverter result;

    for (int r = 0; r < (int)table.size(); ++r)
    {
        for (int c = 0; c < (int)table[r].size(); ++c)
        {
            const std::string &token = table[r][c];
            Phomene p;
            p.Manner = r;
            p.Place = c;
            result.Map[token] = p;
        }
    }
    return result;
}

/**
 * 文字列を変換表に基づいて音素列に変換する
 * @param str 文字列
 * @param table 音素表
 */
std::vector<Phomene> PhonemeConverter::ConvertToPhoneme(const std::string &str)
{
    std::vector<Phomene> output;
    output.reserve(str.length());

    for (size_t i = 0; i < str.length();)
    {
        bool matched = false;
        // 最長一致を優先（最大長が既知ならその値から開始）
        for (size_t len = 2; len > 0; --len)
        {
            if (i + len <= str.length())
            {
                std::string sub = str.substr(i, len);
                auto it = Map.find(sub);
                if (it != Map.end())
                {
                    output.push_back(it->second);
                    i += len;
                    matched = true;
                    break;
                }
            }
        }
        if (!matched)
            i++;
    }
    return output;
}

/**
 * @brief 文字列の配列を言語に変換する
 * @param strs 文字列の配列
 * @param table 音素表
 * @return 言語
 */
Language PhonemeConverter::convertToLanguage(const std::vector<std::string> &strs)
{
    Language result;
    int wordID = 0;
    for (const auto &str : strs)
    {
        Word word;
        word.Form = ConvertToPhoneme(str);
        word.Meanings[str] = 1.0;
        word.ReconstructedWord = word.Form;
        result.Words[wordID] = word;
        wordID++;
    }
    result.Strength = 0.0;
    return result;
}
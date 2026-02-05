#include "Language.h"
#include <iostream>
#include <map>

#include <iostream>

int evolution(
    const int N_BORROW,
    const double P_SOUND_CHANGE,
    const double P_SOUND_LOSS,
    const double P_SEMANTIC_SHIFT,
    const double MAX_SEMANTIC_SHIFT_RATE,
    const double P_WORD_LOSS,
    const double P_WORD_BIRTH,
    const std::wstring &PROTO_LANGUAGE_PATH,
    const std::wstring &PHONEME_TABLE_PATH,
    const std::wstring &MAP_PATH,
    const std::wstring &OUTPUT_PATH)
{
    // ファイル読み込み
    const auto oldTokiPonaData = readCSV(PROTO_LANGUAGE_PATH);
    const auto phoneticsData = readCSV(PHONEME_TABLE_PATH);
    const auto mapData = readCSV(MAP_PATH);

    // データ準備
    if (oldTokiPonaData.empty() || phoneticsData.empty() || mapData.empty())
    {
        return 0;
    }

    auto oldTokiPona = convertToLanguage(oldTokiPonaData[0], phoneticsData);
    const auto mapAdjacentData = getAdjacencies(mapData);
    const auto placeNameData = getNonEmptyStrings(mapData);
    auto languageData = setOldLanguageOnMap(placeNameData, "0", oldTokiPona);

    int generation = -1;
    if (N_BORROW == 0)
    {
        return 0;
    }
    if (oldTokiPona.Words.empty())
    {
        return 0;
    }
    while (true)
    {
        generation++;
        // 言語の影響度を変化させる。
        for (auto &[_, language] : languageData)
        {
            changeLanguageStrength(language);
        }
        // 借用
        for (int i = 0; i < N_BORROW; i++)
        {
            const auto adjucent = mapAdjacentData[getRandomInt(0, mapAdjacentData.size() - 1)];
            bollowWord(languageData, generation, adjucent);
        }
        // 音韻変化
        for (auto &[_, language] : languageData)
        {
            // 音韻変化するかどうか
            if (!getWithProbability(P_SOUND_CHANGE))
            {
                continue;
            }
            // 言語があるか
            if (language.Words.empty())
            {
                continue;
            }
            const auto sound = getRandomSoundFromLanguage(language);
            SoundChange soundChange = makeSoundChangeRandom(sound, phoneticsData, P_SOUND_LOSS);
            changeLanguageSound(language, soundChange);
        }
        // 単語の脱落と新語追加
        for (auto &[_, language] : languageData)
        {
            // 単語が脱落するかどうか
            if (getWithProbability(P_WORD_LOSS))
            {
                removeWordRandom(language, oldTokiPona);
            }
            // 単語を追加するかどうか
            if (getWithProbability(P_WORD_BIRTH))
            {
                createWord(language, oldTokiPona);
            }
        }
        // 単語の意味変化
        for (auto &[_, language] : languageData)
        {
            // 意味変化するかどうか
            if (getWithProbability(P_SEMANTIC_SHIFT))
            {
                changeLanguageMeaning(language, oldTokiPona, MAX_SEMANTIC_SHIFT_RATE);
            }
        }
        // 各位置に言語があれば終了
        bool isThereAllLanguage = true;
        for (auto &[_, language] : languageData)
        {
            if (language.Words.empty())
            {
                isThereAllLanguage = false;
                break;
            }
        }
        if (isThereAllLanguage)
        {
            break;
        }
    }
    // 出力
    exportLanguageToCSV(oldTokiPona, languageData, phoneticsData, OUTPUT_PATH);
    return 0;
}
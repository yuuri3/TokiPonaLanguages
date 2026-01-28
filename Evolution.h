#include "Language.h"
#include <iostream>
#include <map>

int evolution(
    const int N_BOLLOW,
    const double P_SOUND_CHANGE,
    const double P_REMOVE_SOUND,
    const double P_MEANING_CHANGE,
    const double MAX_MEANING_CHANGE_RATE,
    const double P_REMOVE_WORD,
    const double P_CREATE_WORD,
    const std::wstring &OLD_TOKI_PONA,
    const std::wstring &PHONETICS,
    const std::wstring &MAP,
    const std::wstring &TOKI_PONA_LANGUAGES)
{
    // ファイル読み込み
    const auto oldTokiPonaData = readCSV(OLD_TOKI_PONA);
    const auto phoneticsData = readCSV(PHONETICS);
    const auto mapData = readCSV(MAP);

    // データ準備
    if (oldTokiPonaData.empty() || phoneticsData.empty() || mapData.empty())
    {
        return 0;
    }
    const auto oldTokiPona = convertToLanguage(oldTokiPonaData[0], phoneticsData);
    const auto mapAdjacentData = getAdjacencies(mapData);
    const auto placeNameData = getNonEmptyStrings(mapData);
    auto languageData = setOldLanguageOnMap(placeNameData, "0", oldTokiPona);

    int generation = -1;
    if (N_BOLLOW == 0)
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
        for (auto &language : languageData)
        {
            changeLanguageStrength(language);
        }
        // 借用
        for (int i = 0; i < N_BOLLOW; i++)
        {
            const auto adjucent = mapAdjacentData[getRandomInt(0, mapAdjacentData.size() - 1)];
            bollowWord(languageData, generation, adjucent);
        }
        // 音韻変化
        for (auto &language : languageData)
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
            SoundChange soundChange = makeSoundChangeRandom(sound, phoneticsData, P_REMOVE_SOUND);
            changeLanguageSound(language, soundChange);
        }
        // 単語の脱落と新語追加
        for (auto &language : languageData)
        {
            // 単語が脱落するかどうか
            if (getWithProbability(P_REMOVE_WORD))
            {
                removeWordRandom(language, oldTokiPona);
            }
            // 単語を追加するかどうか
            if (getWithProbability(P_CREATE_WORD))
            {
                createWord(language);
            }
        }
        // 単語の意味変化
        for (auto &language : languageData)
        {
            // 意味変化するかどうか
            if (getWithProbability(P_MEANING_CHANGE))
            {
                changeLanguageMeaning(language, oldTokiPona, MAX_MEANING_CHANGE_RATE);
            }
        }
        // 各位置に言語があれば終了
        bool isThereAllLanguage = true;
        for (auto &language : languageData)
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
    exportLanguageToCSV(oldTokiPona, languageData, phoneticsData, TOKI_PONA_LANGUAGES);
    return 0;
}
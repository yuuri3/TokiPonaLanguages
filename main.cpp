#include "header\\Language.h"
#include <iostream>
#include <map>

int main(int argc, char *argv[])
{
    std::string INPUT = argv[1];

    const auto inputData = readCSV(INPUT);
    std::map<std::string, std::string> inputDataMap;
    for (const auto &d : inputData)
    {
        if (d.size() < 2)
        {
            continue;
        }
        inputDataMap[d[0]] = d[1];
    }

    // １世代ごとの借用回数
    const int N_BOLLOW = std::stoi(inputDataMap["N_BOLLOW"]);
    // 音韻変化が起こる確率
    const double P_SOUND_CHANGE = std::stod(inputDataMap["P_SOUND_CHANGE"]);
    // 音韻変化で音が脱落する確率
    const double P_REMOVE_SOUND = std::stod(inputDataMap["P_REMOVE_SOUND"]);
    // 単語が脱落する確率
    const double P_REMOVE_WORD = std::stod(inputDataMap["P_REMOVE_WORD"]);

    // ファイル読み込み
    const std::string OLD_TOKI_PONA = inputDataMap["OLD_TOKI_PONA"];
    const std::string PHONETICS = inputDataMap["PHONETICS"];
    const std::string MAP = inputDataMap["MAP"];
    const auto oldTokiPonaData = readCSV(OLD_TOKI_PONA);
    const auto phoneticsData = readCSV(PHONETICS);
    const auto mapData = readCSV(MAP);

    // データ準備
    const auto oldTokiPona = convertToLanguage(oldTokiPonaData[0], phoneticsData);
    const auto mapAdjacentData = getAdjacencies(mapData);
    const auto placeNameData = getNonEmptyStrings(mapData);
    auto languageData = createConditionalPairs(placeNameData, "0", oldTokiPona);

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
            BollowWord(languageData, generation, adjucent);
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
            const auto sound = getRandomNonEmptyIndex(language);
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
    const auto TOKI_PONA_LANGUAGES = inputDataMap["TOKI_PONA_LANGUAGES"];
    exportLanguageStructToCSV(oldTokiPona, languageData, phoneticsData, TOKI_PONA_LANGUAGES);
    std::cout << "kanryou" << std::endl;
    return 0;
}
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

    auto converter = PhoneticsConverter::Create(phoneticsData);
    auto oldTokiPona = converter.convertToLanguage(oldTokiPonaData[0]);

    LanguageSystem languageSystem;
    languageSystem.Map = mapData;
    languageSystem.PhoneticsMap = phoneticsData;
    languageSystem.SetOldLanguageOnMap("0", oldTokiPona);

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
        languageSystem.ChangeLanguageStrength(1.0);
        // 借用
        languageSystem.BollowWord(N_BORROW, 0.5);
        // 音韻変化
        languageSystem.ChangeLanguageSound(P_SOUND_CHANGE, P_SOUND_LOSS);
        // 単語の脱落と新語追加
        languageSystem.RemoveWordRandom(P_WORD_LOSS);
        languageSystem.CreateWord(P_WORD_BIRTH);
        // 単語の意味変化
        languageSystem.ChangeLanguageMeaning(P_SEMANTIC_SHIFT, MAX_SEMANTIC_SHIFT_RATE);
        // 各位置に言語があれば終了
        if (languageSystem.HasAllPlaceLanguage())
        {
            break;
        }
    }
    // 出力
    languageSystem.ExportLanguageToCSV(OUTPUT_PATH);
    return 0;
}
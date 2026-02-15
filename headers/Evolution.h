#include "Language.h"
#include <iostream>
#include <map>

#include <iostream>

std::optional<LanguageFamily> evolution(
    const int N_LOANWORD,
    const double P_SOUND_CHANGE,
    const double P_SOUND_LOSS,
    const double P_SEMANTIC_SHIFT,
    const double MAX_SEMANTIC_SHIFT_RATE,
    const double P_WORD_LOSS,
    const double P_WORD_BIRTH,
    const std::string &PROTO_LANGUAGE_PATH,
    const std::string &PHONEME_TABLE_PATH,
    const std::string &GEOGRAPHY_PATH,
    const std::string &OUTPUT_PATH)
{
    // ファイル読み込み
    const auto protoLanguageData = readCSV(PROTO_LANGUAGE_PATH);
    const auto phonemeTableData = readCSV(PHONEME_TABLE_PATH);
    const auto geographyData = readCSV(GEOGRAPHY_PATH);

    // データ準備
    if (protoLanguageData.empty() || phonemeTableData.empty() || geographyData.empty())
    {
        return std::nullopt;
    }

    auto converter = PhonemeConverter::Create(phonemeTableData);
    auto protoLanguage = converter.convertToLanguage(protoLanguageData[0]);

    LanguageFamily languageFamily;
    languageFamily.Geography = geographyData;
    languageFamily.PhonemeTable = phonemeTableData;
    languageFamily.SetProtoLanguageOnGeography("0", protoLanguage);

    if (N_LOANWORD == 0)
    {
        return std::nullopt;
    }
    if (protoLanguage.Words.empty())
    {
        return std::nullopt;
    }
    while (true)
    {
        languageFamily.ToNextPeriod();
        // 言語の影響度を変化させる。
        languageFamily.ChangeLanguageStrengthRandom(1.0);
        // 借用
        languageFamily.LoanwordRandom(N_LOANWORD, 0.5);
        // 音韻変化
        languageFamily.PhonologicalChangeRandom(P_SOUND_CHANGE, P_SOUND_LOSS);
        // 単語の脱落と新語追加
        languageFamily.ObsoleteWordRandom(P_WORD_LOSS);
        languageFamily.MakeCompoundRandom(P_WORD_BIRTH);
        // 単語の意味変化
        languageFamily.SemanticChangeRandom(P_SEMANTIC_SHIFT, MAX_SEMANTIC_SHIFT_RATE);
        // 各位置に言語があれば終了
        if (languageFamily.HasAllPlaceLanguage())
        {
            break;
        }
    }
    // 出力
    languageFamily.ExportLanguageToCSV(OUTPUT_PATH);
    languageFamily.Export(OUTPUT_PATH + ".log");
    return languageFamily;
}
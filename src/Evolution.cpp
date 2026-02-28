#include "Evolution.h"

std::optional<LanguageFamilySimulator> evolution(
    const int nLoanword,
    const double pPhonologicalChange,
    const double pSoundLoss,
    const std::string &protoLanguagePath,
    const std::string &phonemeTablePath,
    const std::string &geographyPath,
    const std::string &outputPath)
{
    // ファイル読み込み
    const auto protoLanguageData = readCSV(protoLanguagePath);
    const auto phonemeTableData = readCSV(phonemeTablePath);
    const auto geographyData = readCSV(geographyPath);

    // データ準備
    if (protoLanguageData.empty() || phonemeTableData.empty() || geographyData.empty())
    {
        return std::nullopt;
    }

    auto converter = PhonemeConverter::Create(phonemeTableData);
    auto protoLanguage = converter.convertToLanguage(protoLanguageData[0]);

    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_ = LanguageFamily::Create(geographyData, phonemeTableData);
    simulator.SetProtoLanguageOnGeography("0", protoLanguage);

    if (nLoanword == 0)
    {
        return std::nullopt;
    }
    if (protoLanguage.Empty())
    {
        return std::nullopt;
    }
    while (true)
    {
        simulator.ToNextPeriod();
        // 言語の影響度を変化させる。
        simulator.ChangeLanguageStrengthRandom(1.0);
        // 借用
        simulator.LoanwordRandom(nLoanword, 0.5);
        // 音韻変化
        simulator.PhonologicalChangeRandom(pPhonologicalChange, pSoundLoss);
        // 各位置に言語があれば終了
        if (simulator.HasAllPlaceLanguage())
        {
            break;
        }
    }
    return simulator;
}
#include "Evolution.h"

int main()
{
    // 祖語データなし
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/NoOldLanguage.csv");

    // 音韻データなし
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"",
        L"Map.csv",
        L"ignore/test_data/NoPhonetics.csv");

    // 地図データなし
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"",
        L"ignore/test_data/NoMap.csv");

    // 基準
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/Output.csv");

    // 音韻変化
    evolution(
        1,
        0.1,
        0.1,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/ChangeSound.csv");

    // 音韻変化（音脱落なし）
    evolution(
        1,
        0.1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/ChangeSoundNoRemove.csv");

    // 音韻変化（音脱落のみ）
    evolution(
        1,
        0.1,
        1.0,
        0.0,
        0.0,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/ChangeSoundRemove.csv");

    // 意味変化
    evolution(
        1,
        0.0,
        0.0,
        0.1,
        0.1,
        0.0,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/ChangeMeaning.csv");

    // 単語削除
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.1,
        0.0,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/RemoveWord.csv");

    // 新語追加
    evolution(
        1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.1,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/CreateWord.csv");

    // 意味変化 + 単語削除 + 新語追加
    evolution(
        1,
        0.0,
        0.0,
        0.1,
        0.1,
        0.1,
        0.1,
        L"OldTokiPona.csv",
        L"Phonetics.csv",
        L"Map.csv",
        L"ignore/test_data/ChangeMeaningAndWordNum.csv");
    return 0;
}
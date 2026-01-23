#include "Language.h"
#include <map>
#include <fstream>

std::vector<Phonetics> convertToPhonetics(const std::string &str, const std::vector<std::vector<std::string>> &table)
{
    std::vector<Phonetics> output;

    // 1. 検索を高速化するために Map に変換 (文字列 -> {行, 列})
    std::map<std::string, Phonetics> lookup;
    size_t maxTokenLen = 0; // 変換表にある文字列の最大長を保持

    for (int r = 0; r < (int)table.size(); ++r)
    {
        for (int c = 0; c < (int)table[r].size(); ++c)
        {
            const std::string &token = table[r][c];
            Phonetics p;
            p.Mannar = r;
            p.Place = c;
            lookup[token] = p;
            if (token.length() > maxTokenLen)
            {
                maxTokenLen = token.length();
            }
        }
    }

    // 2. 文字列の解析
    for (size_t i = 0; i < str.length();)
    {
        bool matched = false;

        // 長い一致を優先して検索 (例: "f'" を "f" と "'" より先に探す)
        for (size_t len = maxTokenLen; len > 0; --len)
        {
            if (i + len <= str.length())
            {
                std::string sub = str.substr(i, len);
                if (lookup.count(sub))
                {
                    // 見つかったら行と列を追加
                    output.push_back(lookup[sub]);
                    i += len; // マッチした長さ分進める
                    matched = true;
                    break;
                }
            }
        }

        if (!matched)
        {
            // 変換表にない文字はスキップ（必要に応じてエラー処理）
            i++;
        }
    }

    return output;
}

std::vector<Language> createConditionalPairs(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const std::vector<std::vector<Phonetics>> &language)
{
    std::vector<Language> result;

    for (const std::string &item : mapData)
    {
        // 1番目の要素をitem、2番目を空文字で初期化してペアを作成
        Language p;
        p.Place = item;
        p.Strength = 0.0;
        p.Lang = {};
        p.BollowHistory = {};

        // もし1番目の要素がtargetAと一致したら、2番目をreplacementBにする
        if (item == startPlace)
        {
            p.Lang = language;
        }

        result.push_back(p);
    }

    return result;
}

std::string convertFromCoordinates(const std::vector<Phonetics> &phoneticses, const std::vector<std::vector<std::string>> &table)
{
    std::string result = "";

    // 数列は {行, 列} のペアなので、2ステップずつ進める
    for (size_t i = 0; i < phoneticses.size(); i++)
    {
        int row = phoneticses[i].Mannar;
        int col = phoneticses[i].Place;

        // 範囲外アクセスを防ぐためのチェック
        if (row >= 0 && row < (int)table.size())
        {
            if (col >= 0 && col < (int)table[row].size())
            {
                // 変換表から該当する文字列を取得して連結
                result += table[row][col];
            }
        }
    }

    return result;
}

void changeLanguageSound(
    Language &language,
    const SoundChange &soundChange,
    const bool isProhibiteMinimalPair,
    const bool isSoundDuplication)
{
    Language newLanguage;
    newLanguage.Place = language.Place;
    newLanguage.BollowHistory = language.BollowHistory;
    for (auto &word : language.Lang)
    {
        std::vector<Phonetics> newWord;
        for (size_t i = 0; i < word.size(); i += 1)
        {
            if (word[i] == soundChange.beforePhon)
            {
                if (soundChange.Condition == SoundChangeCondition::Start && i != 0)
                {
                    newWord.push_back(word[i]);
                    continue;
                }
                if (soundChange.Condition == SoundChangeCondition::End && i != word.size() - 1)
                {
                    newWord.push_back(word[i]);
                    continue;
                }
                if (soundChange.Condition == SoundChangeCondition::Middle && (i == 0 || i == word.size() - 1))
                {
                    newWord.push_back(word[i]);
                    continue;
                }
                if (!soundChange.IsRemove)
                {
                    newWord.push_back(soundChange.AfterPhone);
                }
            }
            else
            {
                newWord.push_back(word[i]);
            }
        }
        newLanguage.Lang.emplace_back(newWord);
    }
    // 子音、母音の重複を禁止する処理
    if (isSoundDuplication)
    {
        // 子音と母音の境界
        // 後でファイル入力にする
        constexpr int MAX_CONSONANT_MANNAR = 3;

        for (size_t i = 0; i < newLanguage.Lang.size(); ++i)
        {
            if (newLanguage.Lang[i].empty())
            {
                newLanguage.Lang[i] = language.Lang[i];
            }
            if (newLanguage.Lang[i].size() == 1)
            {
                if (newLanguage.Lang[i][0].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Lang[i] = language.Lang[i];
                }
            }
            else
            {
                bool isDuplicate = false;
                if (newLanguage.Lang[i][0].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Lang[i][1].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Lang[i] = language.Lang[i];
                }
                if (newLanguage.Lang[i][newLanguage.Lang[i].size() - 2].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Lang[i][newLanguage.Lang[i].size() - 1].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Lang[i] = language.Lang[i];
                }
                else
                {
                    for (size_t j = 0; j < newLanguage.Lang[i].size() - 2; j += 1)
                    {
                        if (newLanguage.Lang[i][j].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Lang[i][j + 1].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Lang[i][j + 2].Mannar <= MAX_CONSONANT_MANNAR)
                        {
                            isDuplicate = true;
                        }
                        if (newLanguage.Lang[i][j].Mannar > MAX_CONSONANT_MANNAR && newLanguage.Lang[i][j + 1].Mannar > MAX_CONSONANT_MANNAR && newLanguage.Lang[i][j + 2].Mannar > MAX_CONSONANT_MANNAR)
                        {
                            isDuplicate = true;
                        }
                    }
                }
                if (isDuplicate)
                {
                    newLanguage.Lang[i] = language.Lang[i];
                }
            }
        }
    }
    // ミニマルペアを戻す処理
    if (isProhibiteMinimalPair)
    {
        // 1. 各要素が全体で何回現れるかをカウントする
        // 重複を「先出し」で判定するため、一度全件走査が必要
        std::map<std::vector<Phonetics>, int> counts;
        for (const auto &item : newLanguage.Lang)
        {
            counts[item]++;
        }

        // 2. カウントが1より大きい（重複している）要素をBの要素で置換する
        for (size_t i = 0; i < newLanguage.Lang.size(); ++i)
        {
            // A[i] という値が全体で2回以上現れているかチェック
            if (counts[newLanguage.Lang[i]] > 1)
            {
                // AとBのインデックスを同期させて置き換え
                // BのサイズがAより小さい場合に備えて境界チェックを行う
                if (i < language.Lang.size())
                {
                    newLanguage.Lang[i] = language.Lang[i];
                }
            }
        }
    }
    language = newLanguage;
}

SoundChange makeSoundChangeRandom(const Phonetics &beforePhon, const std::vector<std::vector<std::string>> &table, const double pRemoveSound)
{
    int position = getRandomInt(0, 2);

    SoundChange result;
    result.beforePhon = beforePhon;
    result.IsRemove = getWithProbability(pRemoveSound);
    switch (position)
    {
    case 0:
        result.Condition = SoundChangeCondition::Start;
        break;
    case 1:
        result.Condition = SoundChangeCondition::Middle;
        break;
    case 2:
        result.Condition = SoundChangeCondition::End;
        break;

    default:
        break;
    }
    result.AfterPhone = beforePhon;
    moveRandomOnTable(result.AfterPhone.Mannar, result.AfterPhone.Place, table);
    return result;
}

void exportLanguageStructToCSV(
    const std::vector<struct Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return;

    // 1. ヘッダー行 (Place)
    for (size_t i = 0; i < languages.size(); ++i)
    {
        file << languages[i].Place << ",";
    }
    file << "\n";

    // 2. 最大行数を取得 (各Language.Languageベクトルの最大サイズ)
    size_t maxRows = 0;
    for (const auto &lang : languages)
    {
        maxRows = std::max(maxRows, lang.Lang.size());
    }

    // 3. データ行の出力
    for (size_t r = 0; r < maxRows; ++r)
    {
        for (size_t i = 0; i < languages.size(); ++i)
        {
            // 現在の行(r)が、その言語データの範囲内にあるかチェック
            if (r < languages[i].Lang.size())
            {
                // ここで以前の convertFromCoordinates を呼び出す
                // メンバ名がLanguageなので languages[i].Language となる
                std::string decoded = convertFromCoordinates(languages[i].Lang[r], table);
                file << decoded;
            }

            // カンマの出力
            if (i < languages.size() - 1)
                file << ",";
        }
        file << "\n";
    }

    // 4. 履歴出力
    // int i = 0;
    // while (true)
    // {
    //     bool isExported = false;
    //     for (size_t j = 0; j < languages.size(); j++)
    //     {
    //         if (i < languages[j].BollowHistory.size())
    //         {
    //             std::cout << j << " " << i << std::endl;
    //             file << languages[j].BollowHistory[i];
    //             isExported = true;
    //         }

    //         // カンマの出力
    //         if (j < languages.size() - 1)
    //             file << ",";
    //     }
    //     if (!isExported)
    //     {
    //         break;
    //     }
    //     file << "\n";
    //     i++;
    // }

    file.close();
}

void BollowWord(std::vector<Language> &languages, const int &generation, const std::pair<std::string, std::string> &adjucentData)
{
    std::pair<Language *, Language *> langPair;
    for (size_t i = 0; i < languages.size(); i++)
    {
        if (languages[i].Place == adjucentData.first)
        {
            langPair.first = &(languages[i]);
        }
        else if (languages[i].Place == adjucentData.second)
        {
            langPair.second = &(languages[i]);
        }
    }
    if (langPair.first->Lang.empty() && langPair.second->Lang.empty())
    {
        return;
    }
    else if (!langPair.first->Lang.empty() && langPair.second->Lang.empty())
    {
        langPair.second->Lang = langPair.first->Lang;
        langPair.second->Strength = langPair.first->Strength;
        langPair.second->BollowHistory.push_back({generation, langPair.first->Place});
        return;
    }
    else if (langPair.first->Lang.empty() && !langPair.second->Lang.empty())
    {
        langPair.first->Lang = langPair.second->Lang;
        langPair.first->Strength = langPair.second->Strength;
        langPair.first->BollowHistory.push_back({generation, langPair.second->Place});
        return;
    }
    else if (!langPair.first->Lang.empty() && !langPair.second->Lang.empty())
    {
        if (langPair.first->Strength > langPair.second->Strength)
        {
            for (size_t i = 0; i < langPair.first->Lang.size(); i++)
            {
                if (getRandomInt(0, 1) == 0)
                {
                    langPair.second->Lang[i] = langPair.first->Lang[i];
                    langPair.second->BollowHistory.push_back({generation, langPair.first->Place});
                }
            }
        }
        else
        {
            for (size_t i = 0; i < langPair.first->Lang.size(); i++)
            {
                if (getRandomInt(0, 1) == 0)
                {
                    langPair.first->Lang[i] = langPair.second->Lang[i];
                    langPair.first->BollowHistory.push_back({generation, langPair.second->Place});
                }
            }
        }
    }
}

Phonetics getRandomNonEmptyIndex(const std::vector<std::vector<std::string>> &table)
{
    // 1. 空ではないセルの「座標」をリストに貯める
    std::vector<Phonetics> pool;

    for (int r = 0; r < (int)table.size(); ++r)
    {
        for (int c = 0; c < (int)table[r].size(); ++c)
        {
            if (!table[r][c].empty())
            {
                pool.push_back({r, c});
            }
        }
    }

    // 候補が一つもない場合
    if (pool.empty())
    {
        return {-1, -1};
    }

    // 座標リストのインデックスをランダムに選択
    return pool[getRandomInt(0, pool.size() - 1)];
}

Phonetics getRandomNonEmptyIndex(const Language &language)
{
    if (language.Lang.empty())
    {
        return {0, 0};
    }
    const int index1 = getRandomInt(0, (int)(language.Lang.size()) - 1);
    const int index2 = getRandomInt(0, (int)(language.Lang[index1].size()) - 1);
    return language.Lang[index1][index2];
}

void changeLanguageStrength(Language &language)
{
    language.Strength = language.Strength * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;
}
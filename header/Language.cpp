#include "Language.h"
#include <fstream>
#include <cmath>

namespace
{
    double TOLERANCE = 1.0e-6;
}

Meaning Meaning::Add(const Meaning &meaning) const
{
    Meaning result = *this;
    for (const auto &m : meaning)
    {
        result[m.first] += m.second;
    }
    return result;
}

double Meaning::Dot(const Meaning &meaning) const
{
    Meaning m1 = *(this);
    double result = 0.0;
    for (const auto &m : meaning)
    {
        result += m.second * m1[m.first];
    }
    return result;
}

Meaning Meaning::Product(const double scalar) const
{
    Meaning result = *this;
    for (auto &m : result)
    {
        m.second *= scalar;
    }
    return result;
}

void Meaning::Normalize()
{
    const double norm = std::sqrt(Dot(*this));
    if (norm <= TOLERANCE)
    {
        return;
    }
    *this = Product(1.0 / norm);
}

Word Word::Add(const Word &word) const
{
    Word result;
    result.Sounds = Sounds;
    result.Sounds.insert(result.Sounds.end(), word.Sounds.begin(), word.Sounds.end());
    result.Meanings = Meanings.Add(word.Meanings);
    result.Meanings.Normalize();
    return result;
}

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

Language convertToLanguage(const std::vector<std::string> strs, const std::vector<std::vector<std::string>> &table)
{
    Language result;
    for (const auto &str : strs)
    {
        Word word;
        word.Sounds = convertToPhonetics(str, table);
        word.Meanings[str] = 1.0;
        result.Words.emplace_back(word);
    }
    result.Strength = 0.0;
    result.BollowHistory = {};
    return result;
}

std::vector<Language> setOldLanguageOnMap(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const Language &language)
{
    std::vector<Language> result;

    for (const std::string &item : mapData)
    {
        // 1番目の要素をitem、2番目を空文字で初期化してペアを作成
        Language p;
        p.Strength = 0.0;
        p.Words = {};
        p.BollowHistory = {};

        // もし1番目の要素がtargetAと一致したら、2番目をreplacementBにする
        if (item == startPlace)
        {
            p = language;
        }

        // 位置を設定
        p.Place = item;

        result.push_back(p);
    }

    return result;
}

std::string convertToString(const std::vector<Phonetics> &phoneticses, const std::vector<std::vector<std::string>> &table)
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
    for (auto &word : language.Words)
    {
        Word newWord;
        newWord.Meanings = word.Meanings;
        for (size_t i = 0; i < word.Sounds.size(); i += 1)
        {
            if (word.Sounds[i] == soundChange.beforePhon)
            {
                if (soundChange.Condition == SoundChangeCondition::Start && i != 0)
                {
                    newWord.Sounds.push_back(word.Sounds[i]);
                    continue;
                }
                if (soundChange.Condition == SoundChangeCondition::End && i != word.Sounds.size() - 1)
                {
                    newWord.Sounds.push_back(word.Sounds[i]);
                    continue;
                }
                if (soundChange.Condition == SoundChangeCondition::Middle && (i == 0 || i == word.Sounds.size() - 1))
                {
                    newWord.Sounds.push_back(word.Sounds[i]);
                    continue;
                }
                if (!soundChange.IsRemove)
                {
                    newWord.Sounds.push_back(soundChange.AfterPhone);
                }
            }
            else
            {
                newWord.Sounds.push_back(word.Sounds[i]);
            }
        }
        newLanguage.Words.emplace_back(newWord);
    }
    // 子音、母音の重複を禁止する処理
    if (isSoundDuplication)
    {
        // 子音と母音の境界
        // 後でファイル入力にする
        constexpr int MAX_CONSONANT_MANNAR = 3;

        for (size_t i = 0; i < newLanguage.Words.size(); ++i)
        {
            if (newLanguage.Words[i].Sounds.empty())
            {
                newLanguage.Words[i] = language.Words[i];
            }
            if (newLanguage.Words[i].Sounds.size() == 1)
            {
                if (newLanguage.Words[i].Sounds[0].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Words[i] = language.Words[i];
                }
            }
            else
            {
                bool isDuplicate = false;
                if (newLanguage.Words[i].Sounds[0].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[1].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Words[i] = language.Words[i];
                }
                if (newLanguage.Words[i].Sounds[newLanguage.Words[i].Sounds.size() - 2].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[newLanguage.Words[i].Sounds.size() - 1].Mannar <= MAX_CONSONANT_MANNAR)
                {
                    newLanguage.Words[i] = language.Words[i];
                }
                else
                {
                    for (size_t j = 0; j < newLanguage.Words[i].Sounds.size() - 2; j += 1)
                    {
                        if (newLanguage.Words[i].Sounds[j].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[j + 1].Mannar <= MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[j + 2].Mannar <= MAX_CONSONANT_MANNAR)
                        {
                            isDuplicate = true;
                        }
                        if (newLanguage.Words[i].Sounds[j].Mannar > MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[j + 1].Mannar > MAX_CONSONANT_MANNAR && newLanguage.Words[i].Sounds[j + 2].Mannar > MAX_CONSONANT_MANNAR)
                        {
                            isDuplicate = true;
                        }
                    }
                }
                if (isDuplicate)
                {
                    newLanguage.Words[i] = language.Words[i];
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
        for (const auto &item : newLanguage.Words)
        {
            counts[item.Sounds]++;
        }

        // 2. カウントが1より大きい（重複している）要素をBの要素で置換する
        for (size_t i = 0; i < newLanguage.Words.size(); ++i)
        {
            // A[i] という値が全体で2回以上現れているかチェック
            if (counts[newLanguage.Words[i].Sounds] > 1)
            {
                // AとBのインデックスを同期させて置き換え
                // BのサイズがAより小さい場合に備えて境界チェックを行う
                if (i < language.Words.size())
                {
                    newLanguage.Words[i] = language.Words[i];
                }
            }
        }
    }
    language = newLanguage;
}

void changeLanguageMeaning(
    Language &language,
    const Language &OldLanguage,
    const double maxChangeRate)
{
    if (language.Words.empty())
    {
        return;
    }
    Language newLanguage = language;
    // 単語の意味変化
    const auto index = getRandomInt(0, (int)newLanguage.Words.size() - 1);
    const auto word2 = language.Words[getRandomInt(0, (int)language.Words.size() - 1)];
    const int changeRate = getRandomDouble(0.0, maxChangeRate);
    newLanguage.Words[index].Meanings = newLanguage.Words[index].Meanings.Add(word2.Meanings.Product(changeRate));
    newLanguage.Words[index].Meanings.Normalize();

    Word nearestToWord2OldWord;
    double maxDot = -1.0;
    for (const auto &oldWord : OldLanguage.Words)
    {
        const double dot = word2.Meanings.Dot(oldWord.Meanings);
        if (maxDot < dot)
        {
            maxDot = dot;
            nearestToWord2OldWord = oldWord;
        }
    }
    std::map<Word, std::vector<Word>> mapOldWordToNewWord;
    for (const auto &word : newLanguage.Words)
    {
        Word nearestOldWord;
        maxDot = -1.0;
        for (const auto &oldWord : OldLanguage.Words)
        {
            if (oldWord != nearestToWord2OldWord)
            {
                continue;
            }
            const double dot = word.Meanings.Dot(oldWord.Meanings);
            if (maxDot < dot)
            {
                maxDot = dot;
                nearestOldWord = oldWord;
            }
        }
        mapOldWordToNewWord[nearestOldWord].emplace_back(word);
    }
    for (const auto &m : mapOldWordToNewWord)
    {
        if (m.second.empty())
        {
            return;
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

void exportLanguageToCSV(
    const Language &oldLanguage,
    const std::vector<struct Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return;

    // ヘッダー行 (Place)
    file << ",";
    for (size_t i = 0; i < languages.size(); ++i)
    {
        file << languages[i].Place << ",";
    }
    file << "\n";
    // 祖語の単語との対応
    std::vector<std::map<Word, std::vector<Word>>> mapsOldWordToWord(languages.size());
    for (int i = 0; i < languages.size(); i++)
    {
        const auto language = languages[i];
        for (const auto &word : language.Words)
        {
            Word nearestOldWord;
            double maxDot = -1.0;
            for (const auto &oldWord : oldLanguage.Words)
            {
                const double dot = word.Meanings.Dot(oldWord.Meanings);
                if (maxDot < dot)
                {
                    maxDot = dot;
                    nearestOldWord = oldWord;
                }
            }
            mapsOldWordToWord[i][nearestOldWord].emplace_back(word);
        }
    }
    // 言語名（Toki Pona からの変化）
    file << "Toki Pona" << ",";
    int indexToki, indexPona;
    for (size_t i = 0; i < oldLanguage.Words.size(); i++)
    {
        const auto word = oldLanguage.Words[i];
        if (convertToString(word.Sounds, table) == "toki")
        {
            indexToki = i;
        }
        if (convertToString(word.Sounds, table) == "pona")
        {
            indexPona = i;
        }
    }
    for (size_t i = 0; i < languages.size(); ++i)
    {
        if (mapsOldWordToWord[i][oldLanguage.Words[indexToki]].empty() || mapsOldWordToWord[i][oldLanguage.Words[indexPona]].empty())
        {
            file << ",";
        }
        else
        {
            auto toki = convertToString(mapsOldWordToWord[i][oldLanguage.Words[indexToki]][0].Sounds, table);
            auto pona = convertToString(mapsOldWordToWord[i][oldLanguage.Words[indexPona]][0].Sounds, table);
            toki[0] = std::toupper(toki[0]);
            pona[0] = std::toupper(pona[0]);
            file << toki << " " << pona << ",";
        }
    }
    file << "\n";
    // 単語出力
    for (const auto &oldWord : oldLanguage.Words)
    {
        int maxWordNumNearToOldWord = 0;
        for (size_t i = 0; i < languages.size(); ++i)
        {
            maxWordNumNearToOldWord = std::max(maxWordNumNearToOldWord, (int)mapsOldWordToWord[i][oldWord].size());
        }
        for (int i = 0; i < maxWordNumNearToOldWord; i++)
        {
            if (i == 0)
            {
                file << convertToString(oldWord.Sounds, table);
            }
            file << ",";
            for (size_t j = 0; j < languages.size(); ++j)
            {
                if (i < mapsOldWordToWord[j][oldWord].size())
                {
                    file << convertToString(mapsOldWordToWord[j][oldWord][i].Sounds, table);
                }
                if (j != languages.size() - 1)
                {
                    file << ",";
                }
            }
            file << "\n";
        }
    }

    file.close();
}

void bollowWord(std::vector<Language> &languages, const int &generation, const std::pair<std::string, std::string> &adjucentData)
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
    if (langPair.first->Words.empty() && langPair.second->Words.empty())
    {
        return;
    }
    else if (!langPair.first->Words.empty() && langPair.second->Words.empty())
    {
        langPair.second->Words = langPair.first->Words;
        langPair.second->Strength = langPair.first->Strength;
        langPair.second->BollowHistory.push_back({generation, langPair.first->Place});
        return;
    }
    else if (langPair.first->Words.empty() && !langPair.second->Words.empty())
    {
        langPair.first->Words = langPair.second->Words;
        langPair.first->Strength = langPair.second->Strength;
        langPair.first->BollowHistory.push_back({generation, langPair.second->Place});
        return;
    }
    else if (!langPair.first->Words.empty() && !langPair.second->Words.empty())
    {
        if (langPair.first->Strength > langPair.second->Strength)
        {
            for (size_t i = 0; i < langPair.second->Words.size(); i++)
            {
                Word nearestWord;
                double maxDot = -1.0;
                for (const auto &word : langPair.first->Words)
                {
                    const auto dot = langPair.second->Words[i].Meanings.Dot(word.Meanings);
                    if (maxDot < dot)
                    {
                        maxDot = dot;
                        nearestWord = word;
                    }
                }
                if (getRandomInt(0, 1) == 0)
                {
                    bool isSameSoundWord = false;
                    for (const auto &word : langPair.second->Words)
                    {
                        if (word.Sounds == nearestWord.Sounds)
                        {
                            isSameSoundWord = true;
                        }
                    }
                    if (!isSameSoundWord)
                    {
                        langPair.second->Words[i].Sounds = nearestWord.Sounds;
                    }
                }
            }
            langPair.second->BollowHistory.push_back({generation, langPair.first->Place});
        }
        else
        {
            for (size_t i = 0; i < langPair.first->Words.size(); i++)
            {
                Word nearestWord;
                double maxDot = -1.0;
                for (const auto &word : langPair.second->Words)
                {
                    const auto dot = langPair.first->Words[i].Meanings.Dot(word.Meanings);
                    if (maxDot < dot)
                    {
                        maxDot = dot;
                        nearestWord = word;
                    }
                }
                if (getRandomInt(0, 1) == 0)
                {
                    bool isSameSoundWord = false;
                    for (const auto &word : langPair.first->Words)
                    {
                        if (word.Sounds == nearestWord.Sounds)
                        {
                            isSameSoundWord = true;
                        }
                    }
                    if (!isSameSoundWord)
                    {
                        langPair.first->Words[i].Sounds = nearestWord.Sounds;
                    }
                }
            }
            langPair.first->BollowHistory.push_back({generation, langPair.second->Place});
        }
    }
}

Phonetics getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table)
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

Phonetics getRandomSoundFromLanguage(const Language &language)
{
    if (language.Words.empty())
    {
        return {0, 0};
    }
    const int index1 = getRandomInt(0, (int)(language.Words.size()) - 1);
    const int index2 = getRandomInt(0, (int)(language.Words[index1].Sounds.size()) - 1);
    return language.Words[index1].Sounds[index2];
}

void changeLanguageStrength(Language &language)
{
    language.Strength = language.Strength * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;
}

void removeWordRandom(Language &language, const Language &oldLanguage)
{
    std::map<Word, std::vector<int>> mapOldWordToWordIndex;
    for (int i = 0; i < language.Words.size(); i++)
    {
        const Word word = language.Words[i];
        Word nearestOldWord;
        double maxDot = -1.0;
        for (const auto &oldWord : oldLanguage.Words)
        {
            const double dot = word.Meanings.Dot(oldWord.Meanings);
            if (maxDot < dot)
            {
                maxDot = dot;
                nearestOldWord = oldWord;
            }
        }
        mapOldWordToWordIndex[nearestOldWord].emplace_back(i);
    }

    std::vector<std::vector<int>> wordIndeceHasSameMeaningPair;
    for (const auto &oldWord : oldLanguage.Words)
    {
        if (mapOldWordToWordIndex[oldWord].size() > 1)
        {
            wordIndeceHasSameMeaningPair.emplace_back(mapOldWordToWordIndex[oldWord]);
        }
    }

    if (wordIndeceHasSameMeaningPair.empty())
    {
        return;
    }

    const auto words = wordIndeceHasSameMeaningPair[getRandomInt(0, (int)wordIndeceHasSameMeaningPair.size() - 1)];
    const auto index = words[getRandomInt(0, (int)words.size() - 1)];
    language.Words.erase(language.Words.begin() + index);
}

void createWord(Language &language)
{
    if (language.Words.empty())
    {
        return;
    }
    const auto word1 = language.Words[getRandomInt(0, (int)language.Words.size() - 1)];
    const auto word2 = language.Words[getRandomInt(0, (int)language.Words.size() - 1)];

    const auto newWord = word1.Add(word2);
    language.Words.emplace_back(newWord);
}
#include "Language.h"
#include <fstream>
#include <cmath>
#include <set>

namespace
{
    double TOLERANCE = 1.0e-6;
}

Meaning Meaning::Add(const Meaning &meaning) const
{
    Meaning result = *this;
    for (const auto &[key, value] : meaning)
    {
        result[key] += value;
    }
    return result;
}

double Meaning::Dot(const Meaning &meaning) const
{
    double result = 0.0;

    for (const auto &[key, value] : meaning)
    {
        auto it = find(key);
        if (it != end())
        {
            result += value * it->second;
        }
    }

    return result;
}

Meaning Meaning::Product(const double scalar) const
{
    Meaning result = *this;
    for (auto &[key, value] : result)
    {
        value *= scalar;
    }
    return result;
}

void Meaning::Normalize()
{
    const double dotSelf = Dot(*this);
    if (dotSelf <= TOLERANCE * TOLERANCE) // sqrtの前に判定
    {
        return;
    }
    const double invNorm = 1.0 / std::sqrt(dotSelf);
    for (auto &[key, value] : *this)
    {
        value *= invNorm;
    }
}

// Word::Add: 音素の連結時の再確保を抑制
Word Word::Add(const Word &word) const
{
    Word result;
    result.Sounds.reserve(Sounds.size() + word.Sounds.size());
    result.Sounds.insert(result.Sounds.end(), Sounds.begin(), Sounds.end());
    result.Sounds.insert(result.Sounds.end(), word.Sounds.begin(), word.Sounds.end());

    result.Meanings = Meanings.Add(word.Meanings);
    result.Meanings.Normalize();
    return result;
}

void Word::UpdateNearestProtoWord(const Language &language)
{
    double maxDot = -1.0;
    for (const auto &[_, word] : language.Words)
    {
        const double dot = Meanings.Dot(word.Meanings);
        if (dot > maxDot)
        {
            maxDot = dot;
            NearestProtoWord = word.Sounds;
        }
    }
}

LanguageDifference LanguageDifference::CreateChangeStrength(const int ID, const int section, const double strength)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::ChangeStrength;
    result.IntParam.emplace_back(ID);
    result.DoubleParam.emplace_back(strength);
    return result;
}

LanguageDifference LanguageDifference::CreateChangeSound(const int ID, const int section, const int wordID, const SoundChange soundChange)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::ChangeSound;
    result.IntParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.SoundChanges = soundChange;
    return result;
}

LanguageDifference LanguageDifference::CreateChangeMeaning(const int ID, const int section, const int wordID, const Meaning meaning)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::ChangeMeaning;
    result.IntParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.MeaningChange = meaning;
    return result;
}

LanguageDifference LanguageDifference::CreateBorrowWord(const int ID1, const int ID2, const int section, const int wordID1, const int wordID2)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::BorrowWord;
    result.IntParam.emplace_back(ID1);
    result.IntParam.emplace_back(wordID1);
    result.IntParam.emplace_back(ID2);
    result.IntParam.emplace_back(wordID2);
    return result;
}

LanguageDifference LanguageDifference::CreateAddCompoundWord(const int ID, const int section, const int wordID, const std::vector<int> wordIDs)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::AddCompoundWord;
    result.IntParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    result.IntParam.insert(result.IntParam.end(), wordIDs.begin(), wordIDs.end());
    return result;
}

LanguageDifference LanguageDifference::CreateRemoveWord(const int ID, const int section, const int wordID)
{
    LanguageDifference result;
    result.Section = section;
    result.Type = LanguageDifferenceType::Remove;
    result.IntParam.emplace_back(ID);
    result.IntParam.emplace_back(wordID);
    return result;
}

PhoneticsConverter PhoneticsConverter::Create(const std::vector<std::vector<std::string>> &table)
{
    PhoneticsConverter result;

    for (int r = 0; r < (int)table.size(); ++r)
    {
        for (int c = 0; c < (int)table[r].size(); ++c)
        {
            const std::string &token = table[r][c];
            Phonetics p;
            p.Mannar = r;
            p.Place = c;
            result.Map[token] = p;
        }
    }
    return result;
}

std::vector<Phonetics> PhoneticsConverter::convertToPhonetics(const std::string &str)
{
    std::vector<Phonetics> output;
    output.reserve(str.length());

    // 2. 文字列の解析
    for (size_t i = 0; i < str.length();)
    {
        bool matched = false;

        // 長い一致を優先して検索 (例: "f'" を "f" と "'" より先に探す)
        for (size_t len = 1; len > 0; --len)
        {
            if (i + len <= str.length())
            {
                std::string sub = str.substr(i, len);
                if (Map.count(sub))
                {
                    // 見つかったら行と列を追加
                    output.push_back(Map[sub]);
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

Language PhoneticsConverter::convertToLanguage(const std::vector<std::string> &strs)
{
    Language result;
    int wordID = 0;
    for (const auto &str : strs)
    {
        Word word;
        word.Sounds = convertToPhonetics(str);
        word.Meanings[str] = 1.0;
        word.NearestProtoWord = word.Sounds;
        result.Words[wordID] = word;
        wordID++;
    }
    result.Strength = 0.0;
    return result;
}

std::map<std::string, Language> setOldLanguageOnMap(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const Language &language)
{
    std::map<std::string, Language> result;

    for (const std::string &item : mapData)
    {
        // 1番目の要素をitem、2番目を空文字で初期化してペアを作成
        Language p;
        p.Strength = 0.0;
        p.Words = {};

        // もし1番目の要素がtargetAと一致したら、2番目をreplacementBにする
        if (item == startPlace)
        {
            p = language;
        }

        // 位置を設定
        result[item] = p;
    }

    return result;
}

void LanguageSystem::SetOldLanguageOnMap(
    const std::string &startPlace,
    const Language &language)
{
    LanguageMap = setOldLanguageOnMap(getNonEmptyStrings(Map), startPlace, language);
    ProtoLanguage = language;
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
    // 子音と母音の境界（定数化してループ外で定義）
    constexpr int MAX_CONSONANT_MANNAR = 3;

    // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
    std::map<int, Word> updatedWords;

    // 1. 音韻変化の適用と音素重複チェックを同時に行う
    for (auto &[wordID, word] : language.Words)
    {
        bool changed = false;
        std::vector<Phonetics> nextSounds;
        nextSounds.reserve(word.Sounds.size()); // メモリ確保を1回に抑制

        for (size_t i = 0; i < word.Sounds.size(); ++i)
        {
            const auto &currentPhon = word.Sounds[i];

            // 変化条件の判定
            bool isMatch = (currentPhon == soundChange.beforePhon);
            if (isMatch)
            {
                if (soundChange.Condition == SoundChangeCondition::Start && i != 0)
                    isMatch = false;
                else if (soundChange.Condition == SoundChangeCondition::End && i != word.Sounds.size() - 1)
                    isMatch = false;
                else if (soundChange.Condition == SoundChangeCondition::Middle && (i == 0 || i == word.Sounds.size() - 1))
                    isMatch = false;
            }

            if (isMatch)
            {
                changed = true;
                if (!soundChange.IsRemove)
                {
                    nextSounds.push_back(soundChange.AfterPhone);
                }
            }
            else
            {
                nextSounds.push_back(currentPhon);
            }
        }

        if (!changed)
            continue;

        // 子音・母音の重複禁止チェック (isSoundDuplication)
        if (isSoundDuplication)
        {
            bool isInvalid = false;
            if (nextSounds.empty())
                isInvalid = true;
            else if (nextSounds.size() == 1)
            {
                if (nextSounds[0].Mannar <= MAX_CONSONANT_MANNAR)
                    isInvalid = true;
            }
            else
            {
                // 境界条件のチェック
                if ((nextSounds[0].Mannar <= MAX_CONSONANT_MANNAR && nextSounds[1].Mannar <= MAX_CONSONANT_MANNAR) ||
                    (nextSounds.back().Mannar <= MAX_CONSONANT_MANNAR && nextSounds[nextSounds.size() - 2].Mannar <= MAX_CONSONANT_MANNAR))
                {
                    isInvalid = true;
                }
                else
                {
                    // 3連続のチェック
                    for (size_t j = 0; j + 2 < nextSounds.size(); ++j)
                    {
                        bool isConsonant = (nextSounds[j].Mannar <= MAX_CONSONANT_MANNAR &&
                                            nextSounds[j + 1].Mannar <= MAX_CONSONANT_MANNAR &&
                                            nextSounds[j + 2].Mannar <= MAX_CONSONANT_MANNAR);
                        bool isVowel = (nextSounds[j].Mannar > MAX_CONSONANT_MANNAR &&
                                        nextSounds[j + 1].Mannar > MAX_CONSONANT_MANNAR &&
                                        nextSounds[j + 2].Mannar > MAX_CONSONANT_MANNAR);
                        if (isConsonant || isVowel)
                        {
                            isInvalid = true;
                            break;
                        }
                    }
                }
            }
            if (isInvalid)
                continue; // 違反していればこの単語の変化は破棄
        }

        // 変化後の単語候補を一時保存
        Word newWord = word;
        newWord.Sounds = std::move(nextSounds); // 所有権を移転してコピーを回避
        updatedWords[wordID] = std::move(newWord);
    }

    // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
    if (isProhibiteMinimalPair)
    {
        // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
        std::map<std::vector<Phonetics>, int> soundCounts;
        for (const auto &[wordID, word] : language.Words)
        {
            auto it = updatedWords.find(wordID);
            soundCounts[it != updatedWords.end() ? it->second.Sounds : word.Sounds]++;
        }

        // 重複が発生する変化を差し止める
        for (auto it = updatedWords.begin(); it != updatedWords.end();)
        {
            if (soundCounts[it->second.Sounds] > 1)
                it = updatedWords.erase(it);
            else
                ++it;
        }
    }

    // 3. 最終的な反映（一括代入）
    for (auto &[wordID, newWord] : updatedWords)
    {
        language.Words[wordID] = std::move(newWord);
    }
}

void LanguageSystem::ChangeLanguageSound(
    const double pSoundChange,
    const double pSoundLoss,
    const bool isProhibitMinimalPair,
    const bool isSoundDuplication)
{
    for (auto &[_, language] : LanguageMap)
    {
        // 音韻変化するかどうか
        if (!getWithProbability(pSoundChange))
        {
            continue;
        }
        // 言語があるか
        if (language.Words.empty())
        {
            continue;
        }
        const auto sound = getRandomSoundFromLanguage(language);
        SoundChange soundChange = makeSoundChangeRandom(sound, PhoneticsMap, pSoundLoss);
        changeLanguageSound(language, soundChange, isProhibitMinimalPair, isSoundDuplication);
    }
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
    newLanguage.Words[index].UpdateNearestProtoWord(OldLanguage);

    // 祖語と最低１単語の意味が対応するようにする。
    // newLanguage に対応する単語がある祖語の単語のセット
    std::set<std::vector<Phonetics>> NewToProtoWordSet;
    for (const auto &[_, word] : newLanguage.Words)
    {
        NewToProtoWordSet.insert(word.NearestProtoWord);
    }
    if (language.Words.size() == NewToProtoWordSet.size())
    {
        language = newLanguage;
    }
}

void LanguageSystem::ChangeLanguageMeaning(
    const double pSemanticShift,
    const double maxSemanticShiftRate)
{
    for (auto &[_, language] : LanguageMap)
    {
        // 意味変化するかどうか
        if (getWithProbability(pSemanticShift))
        {
            changeLanguageMeaning(language, ProtoLanguage, maxSemanticShiftRate);
        }
    }
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
    Language &oldLanguage,
    const std::map<std::string, Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::wstring &filename)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
        return;

    // 1. 文字列変換の結果をキャッシュするマップ (高速化の肝)
    std::map<std::vector<Phonetics>, std::string> stringCache;
    auto getCachedString = [&](const std::vector<Phonetics> &s) -> const std::string &
    {
        auto it = stringCache.find(s);
        if (it != stringCache.end())
            return it->second;
        return stringCache[s] = convertToString(s, table); //
    };

    // 2. ヘッダー行 (Place) の出力と、Languageポインタのキャッシュ
    file << ",";
    std::vector<const Language *> langPtrList;
    langPtrList.reserve(languages.size());
    for (const auto &[place, language] : languages)
    {
        file << place << ",";
        langPtrList.push_back(&language); // ループ内でのmap検索を避けるために保持
    }
    file << "\n";

    // 3. 祖語の単語との対応マップの作成
    // mapsOldWordToWord[言語インデックス][祖語の音素列] -> 該当する単語リスト
    std::vector<std::map<std::vector<Phonetics>, std::vector<const Word *>>> mapsOldWordToWord;
    mapsOldWordToWord.resize(langPtrList.size());
    for (size_t i = 0; i < langPtrList.size(); ++i)
    {
        for (const auto &[_, word] : langPtrList[i]->Words)
        {
            mapsOldWordToWord[i][word.NearestProtoWord].push_back(&word); //
        }
    }

    // 4. 「Toki Pona」行の出力 (言語名の特定)
    int indexToki = -1, indexPona = -1;
    for (const auto &[id, word] : oldLanguage.Words)
    {
        std::string s = getCachedString(word.Sounds);
        if (s == "toki")
            indexToki = id;
        if (s == "pona")
            indexPona = id;
    }

    file << "Toki Pona,";
    if (indexToki != -1 && indexPona != -1)
    {
        const auto &tokiSounds = oldLanguage.Words[indexToki].Sounds;
        const auto &ponaSounds = oldLanguage.Words[indexPona].Sounds;

        for (size_t i = 0; i < langPtrList.size(); ++i)
        {
            const auto &tokiList = mapsOldWordToWord[i][tokiSounds];
            const auto &ponaList = mapsOldWordToWord[i][ponaSounds];

            if (tokiList.empty() || ponaList.empty())
            {
                file << ",";
            }
            else
            {
                std::string tokiStr = getCachedString(tokiList[0]->Sounds);
                std::string ponaStr = getCachedString(ponaList[0]->Sounds);
                if (!tokiStr.empty())
                    tokiStr[0] = std::toupper(tokiStr[0]);
                if (!ponaStr.empty())
                    ponaStr[0] = std::toupper(ponaStr[0]);
                file << tokiStr << " " << ponaStr << ",";
            }
        }
    }
    file << "\n";

    // 5. 各単語の出力
    for (const auto &[id, oldWord] : oldLanguage.Words)
    {
        const auto &protoSounds = oldWord.Sounds;

        // この祖語単語に対して、各地点で最大何個の派生語があるか確認
        size_t maxRows = 0;
        for (size_t i = 0; i < langPtrList.size(); ++i)
        {
            maxRows = std::max(maxRows, mapsOldWordToWord[i][protoSounds].size());
        }

        // 派生語の数だけ行を出力
        for (size_t row = 0; row < maxRows; ++row)
        {
            if (row == 0)
                file << getCachedString(protoSounds); // 最初の行だけ祖語を表示
            file << ",";

            for (size_t langIdx = 0; langIdx < langPtrList.size(); ++langIdx)
            {
                const auto &derivedWords = mapsOldWordToWord[langIdx][protoSounds];
                if (row < derivedWords.size())
                {
                    file << getCachedString(derivedWords[row]->Sounds);
                }
                file << (langIdx == langPtrList.size() - 1 ? "" : ",");
            }
            file << "\n";
        }
    }

    file.close();
}

void LanguageSystem::ExportLanguageToCSV(const std::wstring &filename)
{
    exportLanguageToCSV(ProtoLanguage, LanguageMap, PhoneticsMap, filename);
}

// bollowWord: O(N^2)のループ内で重複する計算を削減
void bollowWord(std::map<std::string, Language> &languages, const int &generation, const std::pair<std::string, std::string> &adjucentData)
{
    auto it1 = languages.find(adjucentData.first);
    auto it2 = languages.find(adjucentData.second);
    if (it1 == languages.end() || it2 == languages.end())
        return;

    Language &l1 = it1->second;
    Language &l2 = it2->second;

    if (l1.Words.empty() || l2.Words.empty())
    {
        if (l1.Words.empty())
        {
            l1.Words = l2.Words;
            l1.Strength = l2.Strength;
        }
        else
        {
            l2.Words = l1.Words;
            l2.Strength = l1.Strength;
        }
        return;
    }

    auto *source = (l1.Strength > l2.Strength) ? &l1 : &l2;
    auto *target = (l1.Strength > l2.Strength) ? &l2 : &l1;

    for (auto &[tID, tWord] : target->Words)
    {
        if (getRandomInt(0, 1) != 0)
            continue;

        const Word *bestSourceWord = nullptr;
        double maxDot = -1.0;

        for (const auto &[sID, sWord] : source->Words)
        {
            double dot = tWord.Meanings.Dot(sWord.Meanings);
            if (dot > maxDot)
            {
                maxDot = dot;
                bestSourceWord = &sWord;
            }
        }

        if (bestSourceWord)
        {
            // 同音語チェックを最適化
            bool isDuplicate = false;
            for (const auto &[checkID, checkWord] : target->Words)
            {
                if (checkWord.Sounds == bestSourceWord->Sounds)
                {
                    isDuplicate = true;
                    break;
                }
            }
            if (!isDuplicate)
                tWord.Sounds = bestSourceWord->Sounds;
        }
    }
}

void LanguageSystem::BollowWord(const int nBorrow, const double pBorrow)
{
    const auto mapAdjacentData = getAdjacencies(Map);
    for (int i = 0; i < nBorrow; i++)
    {
        // 借用率 は現在固定
        const auto adjucent = mapAdjacentData[getRandomInt(0, mapAdjacentData.size() - 1)];
        bollowWord(LanguageMap, 0, adjucent);
    }
}

Phonetics getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table)
{
    // 1. 空ではないセルの「座標」をリストに貯める
    std::vector<Phonetics> pool;
    pool.reserve(table.size() * table[0].size());

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

Phonetics getRandomSoundFromLanguage(Language &language)
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

void LanguageSystem::ChangeLanguageStrength(const double pChangeStrength)
{
    for (auto &[_, language] : LanguageMap)
    {
        if (getWithProbability(pChangeStrength))
        {
            changeLanguageStrength(language);
        }
    }
}

// removeWordRandom: mapのerase(index)は誤りのため、イテレータによる削除に修正
void removeWordRandom(Language &language, const Language &oldLanguage)
{
    if (language.Words.empty())
        return;

    std::map<std::vector<Phonetics>, std::vector<int>> mapProtoWordToWordIndice;
    for (const auto &[id, word] : language.Words)
    {
        mapProtoWordToWordIndice[word.NearestProtoWord].push_back(id);
    }

    std::vector<int> duplicatedIds;
    for (const auto &[key, ids] : mapProtoWordToWordIndice)
    {
        if (ids.size() > 1)
            duplicatedIds.insert(duplicatedIds.end(), ids.begin(), ids.end());
    }

    if (!duplicatedIds.empty())
    {
        int targetId = duplicatedIds[getRandomInt(0, duplicatedIds.size() - 1)];
        language.Words.erase(targetId); // mapのキー指定削除はO(log N)
    }
}

void LanguageSystem::RemoveWordRandom(const double pWordLoss)
{
    for (auto &[_, language] : LanguageMap)
    {
        // 単語が脱落するかどうか
        if (getWithProbability(pWordLoss))
        {
            removeWordRandom(language, ProtoLanguage);
        }
    }
}

void createWord(Language &language, const Language &oldLanguage)
{
    if (language.Words.empty())
    {
        return;
    }
    const auto word1 = language.Words[getRandomInt(0, (int)language.Words.size() - 1)];
    const auto word2 = language.Words[getRandomInt(0, (int)language.Words.size() - 1)];

    auto newWord = word1.Add(word2);
    newWord.UpdateNearestProtoWord(oldLanguage);
    if (language.Words.empty())
    {
        language.Words[0] = newWord;
    }
    else
    {
        const int newWordId = language.Words.rbegin()->first + 1;
        language.Words[newWordId] = newWord;
    }
}

void LanguageSystem::CreateWord(const double pWordBirth)
{
    for (auto &[_, language] : LanguageMap)
    {
        // 単語を追加するかどうか
        if (getWithProbability(pWordBirth))
        {
            createWord(language, ProtoLanguage);
        }
    }
}

bool LanguageSystem::HasAllPlaceLanguage()
{
    for (const auto &place : getNonEmptyStrings(Map))
    {
        if (LanguageMap.count(place) == 0)
        {
            return false;
        }
        if (LanguageMap[place].Words.empty())
        {
            return false;
        }
    }
    return true;
}
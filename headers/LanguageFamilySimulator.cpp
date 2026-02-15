#include "LanguageFamilySimulator.h"
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace
{

    // ヘルパー：vectorの中身を文字列に変換
    template <typename T>
    std::string joinVector(const std::vector<T> &vec, const std::string &del = " ")
    {
        std::stringstream ss;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ss << vec[i] << (i == vec.size() - 1 ? "" : del);
        }
        return ss.str();
    }

    // ヘルパー関数：列挙型を文字列に変換（可読性向上）
    std::string getDiffTypeName(LanguageDifferenceType type)
    {
        switch (type)
        {
        case LanguageDifferenceType::ChangeStrength:
            return "ChangeStrength";
        case LanguageDifferenceType::PhonologicalChange:
            return "ChangeSound";
        case LanguageDifferenceType::SemanticChange:
            return "ChangeMeaning";
        case LanguageDifferenceType::Loanword:
            return "BorrowWord";
        case LanguageDifferenceType::AddCompound:
            return "AddCompoundWord";
        case LanguageDifferenceType::ObsoleteWord:
            return "Remove";
        default:
            return "Unknown";
        }
    }
}

std::map<std::string, Language> setProtoLanguageOnMap(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const Language &protoLanguage)
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
            p = protoLanguage;
        }

        // 位置を設定
        result[item] = p;
    }

    return result;
}

/**
 * 地図データの特定の位置に祖語を配置する
 * @param startPlace 祖語を配置する位置
 * @param protoLanguage 祖語
 */
void LanguageFamilySimulator::SetProtoLanguageOnGeography(
    const std::string &startPlace,
    const Language &protoLanguage)
{
    Languages = setProtoLanguageOnMap(getNonEmptyStrings(LanguageFamily_.Geography), startPlace, protoLanguage);
    ProtoLanguage = protoLanguage;

    // ログ
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);
    LanguageFamily_.languageDifference.emplace_back(LanguageDifference::CreateChangeStrength(startPlace, Period, protoLanguage.Strength));
    for (const auto &[ID, word] : protoLanguage.Words)
    {
        LanguageFamily_.languageDifference.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period, ID, converter.ConvertToString(word.GetForm())));
    }
}

/**
 * @brief Get the 単語 object
 *
 * @param place 位置
 * @return std::vector<std::string>
 */
std::vector<std::string> LanguageFamilySimulator::GetWords(std::string place)
{
    if (Languages.count(place) == 0)
    {
        return {};
    }
    const auto language = Languages[place];
    std::vector<std::string> result;
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);
    for (const auto &[_, word] : language.Words)
    {
        result.emplace_back(converter.ConvertToString(word.GetForm()));
    }
    return result;
}

/**
 * 音素列を変換表に基づいて文字列に復元する
 * @param Phonemes 音素列
 * @param table 音素表
 */
std::string PhonemeConverter::ConvertToString(const std::vector<Phomene> &phonemes)
{
    std::string result = "";

    for (const auto &phoneme : phonemes)
    {
        for (const auto &[keyString, valuePhoneme] : Map)
        {
            if (phoneme == valuePhoneme)
            {
                result += keyString;
            }
        }
    }

    return result;
}

/**
 * 音変化
 * @param pPhonologicalChange 音韻変化確率
 * @param pSoundLoss 音素脱落確率
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 * @param isSoundDuplication 音素の重複を禁止するか
 *
 * @note ある言語の単語を一斉に変化させる。
 */
void LanguageFamilySimulator::PhonologicalChangeRandom(
    const double pPhonologicalChange,
    const double pSoundLoss,
    const bool isProhibitMinimalPair,
    const bool isSoundDuplication)
{
    for (auto &[ID, language] : Languages)
    {
        // 音韻変化するかどうか
        if (!getWithProbability(pPhonologicalChange))
        {
            continue;
        }
        // 言語があるか
        if (language.Words.empty())
        {
            continue;
        }
        const auto sound = getRandomSoundFromLanguage(language);
        PhonologicalChange phonologicalChange = makepPhonologicalChangeRandom(sound, LanguageFamily_.PhonemeTable, pSoundLoss);
        // changeLanguageSound(language, soundChange, isProhibitMinimalPair, isSoundDuplication);
        {
            // 子音と母音の境界（定数化してループ外で定義）
            constexpr int MAX_CONSONANT_MANNER = 3;

            // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
            std::map<int, WordForSimulation> updatedWords;

            // 1. 音韻変化の適用と音素重複チェックを同時に行う
            for (auto &[wordID, word] : language.Words)
            {
                bool changed = false;
                std::vector<Phomene> nextSounds;
                nextSounds.reserve(word.GetForm().size()); // メモリ確保を1回に抑制

                for (size_t i = 0; i < word.GetForm().size(); ++i)
                {
                    const auto &currentPhon = word.GetForm()[i];

                    // 変化条件の判定
                    bool isMatch = (currentPhon == phonologicalChange.BeforePhoneme);
                    if (isMatch)
                    {
                        if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Start && i != 0)
                            isMatch = false;
                        else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::End && i != word.GetForm().size() - 1)
                            isMatch = false;
                        else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Middle && (i == 0 || i == word.GetForm().size() - 1))
                            isMatch = false;
                    }

                    if (isMatch)
                    {
                        changed = true;
                        if (!phonologicalChange.IsRemove)
                        {
                            nextSounds.push_back(phonologicalChange.AfterPhoneme);
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
                        if (nextSounds[0].Manner <= MAX_CONSONANT_MANNER)
                            isInvalid = true;
                    }
                    else
                    {
                        // 境界条件のチェック
                        if ((nextSounds[0].Manner <= MAX_CONSONANT_MANNER && nextSounds[1].Manner <= MAX_CONSONANT_MANNER) ||
                            (nextSounds.back().Manner <= MAX_CONSONANT_MANNER && nextSounds[nextSounds.size() - 2].Manner <= MAX_CONSONANT_MANNER))
                        {
                            isInvalid = true;
                        }
                        else
                        {
                            // 3連続のチェック
                            for (size_t j = 0; j + 2 < nextSounds.size(); ++j)
                            {
                                bool isConsonant = (nextSounds[j].Manner <= MAX_CONSONANT_MANNER &&
                                                    nextSounds[j + 1].Manner <= MAX_CONSONANT_MANNER &&
                                                    nextSounds[j + 2].Manner <= MAX_CONSONANT_MANNER);
                                bool isVowel = (nextSounds[j].Manner > MAX_CONSONANT_MANNER &&
                                                nextSounds[j + 1].Manner > MAX_CONSONANT_MANNER &&
                                                nextSounds[j + 2].Manner > MAX_CONSONANT_MANNER);
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
                WordForSimulation newWord = word;
                newWord.GetForm() = std::move(nextSounds); // 所有権を移転してコピーを回避
                updatedWords[wordID] = std::move(newWord);
            }

            // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
            if (isProhibitMinimalPair)
            {
                // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
                std::map<std::vector<Phomene>, int> soundCounts;
                for (const auto &[wordID, word] : language.Words)
                {
                    auto it = updatedWords.find(wordID);
                    soundCounts[it != updatedWords.end() ? it->second.GetForm() : word.GetForm()]++;
                }

                // 重複が発生する変化を差し止める
                for (auto it = updatedWords.begin(); it != updatedWords.end();)
                {
                    if (soundCounts[it->second.GetForm()] > 1)
                        it = updatedWords.erase(it);
                    else
                        ++it;
                }
            }

            // 3. 最終的な反映（一括代入）
            for (auto &[wordID, newWord] : updatedWords)
            {
                language.Words[wordID] = std::move(newWord);

                // ログ
                const auto dif = LanguageDifference::CreatePhonologicalChange(ID, Period, wordID, phonologicalChange);
                LanguageFamily_.languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 意味変化
 *
 * @param pSemanticShift 意味変化確率
 * @param maxChangeRate 意味変化大きさ
 *
 * @note 単語１つの意味を変化させる
 */
void LanguageFamilySimulator::SemanticChangeRandom(
    const double pSemanticShift,
    const double maxSemanticShiftRate)
{
    for (auto &[ID, language] : Languages)
    {
        // 意味変化するかどうか
        if (getWithProbability(pSemanticShift))
        {
            if (language.Words.empty())
                return;

            // 変更対象の単語をランダムに選択
            // マップの要素にランダムアクセスするため、イテレータを進める
            int targetIdx = getRandomInt(0, (int)language.Words.size() - 1);
            auto it = language.Words.begin();
            std::advance(it, targetIdx);
            const auto wordID = it->first;
            WordForSimulation &targetWord = it->second;

            // 変化の種となる単語をもう一つ選択
            int seedIdx = getRandomInt(0, (int)language.Words.size() - 1);
            auto itSeed = language.Words.begin();
            std::advance(itSeed, seedIdx);
            const WordForSimulation &seedWord = itSeed->second;

            // 現在の状態を保存（ロールバック用）
            Meaning oldMeaning = targetWord.Meanings;
            std::vector<Phomene> oldProto = targetWord.ReconstructedWord;

            // 意味の変化を適用
            double changeRate = getRandomDouble(0.0, maxSemanticShiftRate);
            targetWord.Meanings = targetWord.Meanings.Add(seedWord.Meanings.Product(changeRate));
            targetWord.Meanings.Normalize();
            targetWord.UpdateReconstructedWord(ProtoLanguage);

            // 整合性チェック：すべての単語が異なる祖語に対応しているか（単射性の維持）
            // 巨大なセットを作る代わりに、他の単語と衝突していないかだけをチェック
            bool isConflict = false;
            for (auto checkIt = language.Words.begin(); checkIt != language.Words.end(); ++checkIt)
            {
                if (checkIt == it)
                    continue; // 自分自身はスキップ
                if (checkIt->second.ReconstructedWord == targetWord.ReconstructedWord)
                {
                    isConflict = true;
                    break;
                }
            }

            // 衝突が発生した場合は元の状態に戻す
            if (isConflict)
            {
                targetWord.Meanings = std::move(oldMeaning);
                targetWord.ReconstructedWord = std::move(oldProto);
            }
            else
            {
                // ログ
                const auto dif = LanguageDifference::CreateSemanticChange(ID, Period, wordID, seedWord.Meanings);
                LanguageFamily_.languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * 変化規則をランダムに生成
 * @param beforePlace 変化前音素
 * @param beforeMannar 変化前音素
 * @param table 音素表
 * @param pRemovePhoneme 音が脱落する確率
 */
PhonologicalChange makepPhonologicalChangeRandom(const Phomene &beforePhon, const std::vector<std::vector<std::string>> &table, const double pRemoveSound)
{
    int position = getRandomInt(0, 2);

    PhonologicalChange result;
    result.BeforePhoneme = beforePhon;
    result.IsRemove = getWithProbability(pRemoveSound);
    switch (position)
    {
    case 0:
        result.PhoneticEnvironment = PhoneticEnvironment::Start;
        break;
    case 1:
        result.PhoneticEnvironment = PhoneticEnvironment::Middle;
        break;
    case 2:
        result.PhoneticEnvironment = PhoneticEnvironment::End;
        break;

    default:
        break;
    }
    result.AfterPhoneme = beforePhon;
    moveRandomOnTable(result.AfterPhoneme.Manner, result.AfterPhoneme.Place, table);
    return result;
}

void exportLanguageToCSV(
    Language &protoLanguage,
    const std::map<std::string, Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::string &filename)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
        return;

    // 1. 文字列変換の結果をキャッシュするマップ (高速化の肝)
    std::map<std::vector<Phomene>, std::string> stringCache;
    auto getCachedString = [&](const std::vector<Phomene> &s) -> const std::string &
    {
        auto it = stringCache.find(s);
        if (it != stringCache.end())
            return it->second;
        PhonemeConverter converter = PhonemeConverter::Create(table);
        return stringCache[s] = converter.ConvertToString(s); //
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
    // mapsReconstructedWordToWord[言語インデックス][祖語の音素列] -> 該当する単語リスト
    std::vector<std::map<std::vector<Phomene>, std::vector<const WordForSimulation *>>> mapsReconstructedWordToWord;
    mapsReconstructedWordToWord.resize(langPtrList.size());
    for (size_t i = 0; i < langPtrList.size(); ++i)
    {
        for (const auto &[_, word] : langPtrList[i]->Words)
        {
            mapsReconstructedWordToWord[i][word.ReconstructedWord].push_back(&word); //
        }
    }

    // 4. 「Toki Pona」行の出力 (言語名の特定)
    int indexToki = -1, indexPona = -1;
    for (const auto &[id, word] : protoLanguage.Words)
    {
        std::string s = getCachedString(word.GetForm());
        if (s == "toki")
            indexToki = id;
        if (s == "pona")
            indexPona = id;
    }

    file << "Toki Pona,";
    if (indexToki != -1 && indexPona != -1)
    {
        const auto &tokiSounds = protoLanguage.Words[indexToki].GetForm();
        const auto &ponaSounds = protoLanguage.Words[indexPona].GetForm();

        for (size_t i = 0; i < langPtrList.size(); ++i)
        {
            const auto &tokiList = mapsReconstructedWordToWord[i][tokiSounds];
            const auto &ponaList = mapsReconstructedWordToWord[i][ponaSounds];

            if (tokiList.empty() || ponaList.empty())
            {
                file << ",";
            }
            else
            {
                std::string tokiStr = getCachedString(tokiList[0]->GetForm());
                std::string ponaStr = getCachedString(ponaList[0]->GetForm());
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
    for (const auto &[id, reconstructedWord] : protoLanguage.Words)
    {
        const auto &protoSounds = reconstructedWord.GetForm();

        // この祖語単語に対して、各地点で最大何個の派生語があるか確認
        size_t maxRows = 0;
        for (size_t i = 0; i < langPtrList.size(); ++i)
        {
            maxRows = std::max(maxRows, mapsReconstructedWordToWord[i][protoSounds].size());
        }

        // 派生語の数だけ行を出力
        for (size_t row = 0; row < maxRows; ++row)
        {
            if (row == 0)
                file << getCachedString(protoSounds); // 最初の行だけ祖語を表示
            file << ",";

            for (size_t langIdx = 0; langIdx < langPtrList.size(); ++langIdx)
            {
                const auto &derivedWords = mapsReconstructedWordToWord[langIdx][protoSounds];
                if (row < derivedWords.size())
                {
                    file << getCachedString(derivedWords[row]->GetForm());
                }
                file << (langIdx == langPtrList.size() - 1 ? "" : ",");
            }
            file << "\n";
        }
    }

    file.close();
}

/**
 * Language構造体のリストをCSVに出力する
 * @param filename 出力ファイル名
 */
void LanguageFamilySimulator::ExportLanguageToCSV(const std::string &filename)
{
    exportLanguageToCSV(ProtoLanguage, Languages, LanguageFamily_.PhonemeTable, filename);
}

/**
 * 単語を借用
 *
 * @param nLoanword 借用回数
 * @param pLoanword 借用率
 *
 * @note 借用の履歴をlanguageに記録
 */
void LanguageFamilySimulator::LoanwordRandom(const int nLoanword, const double pLoanword)
{
    const auto mapAdjacentData = getAdjacencies(LanguageFamily_.Geography);
    for (int i = 0; i < nLoanword; i++)
    {
        // 借用率 は現在固定
        const auto adjucent = mapAdjacentData[getRandomInt(0, mapAdjacentData.size() - 1)];
        {
            auto it1 = Languages.find(adjucent.first);
            auto it2 = Languages.find(adjucent.second);
            if (it1 == Languages.end() || it2 == Languages.end())
                return;

            Language &l1 = it1->second;
            Language &l2 = it2->second;

            if (l1.Words.empty() || l2.Words.empty())
            {
                if (l1.Words.empty())
                {
                    l1.Words = l2.Words;
                    l1.Strength = l2.Strength;

                    // ログ
                    for (const auto &[wordID, word] : l2.Words)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(it2->first, it1->first, Period, wordID, wordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
                else
                {
                    l2.Words = l1.Words;
                    l2.Strength = l1.Strength;

                    // ログ
                    for (const auto &[wordID, word] : l1.Words)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(it1->first, it2->first, Period, wordID, wordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
                return;
            }

            auto *source = (l1.Strength > l2.Strength) ? &l1 : &l2;
            auto *target = (l1.Strength > l2.Strength) ? &l2 : &l1;
            const auto sID = (l1.Strength > l2.Strength) ? it1->first : it2->first;
            const auto tID = (l1.Strength > l2.Strength) ? it2->first : it1->first;

            for (auto &[tWordID, tWord] : target->Words)
            {
                if (getRandomInt(0, 1) != 0)
                    continue;

                const WordForSimulation *bestSourceWord = nullptr;
                int bestSourceWordID = -1;
                double maxDot = -1.0;

                for (const auto &[sWordID, sWord] : source->Words)
                {
                    double dot = tWord.Meanings.Dot(sWord.Meanings);
                    if (dot > maxDot)
                    {
                        maxDot = dot;
                        bestSourceWord = &sWord;
                        bestSourceWordID = sWordID;
                    }
                }

                if (bestSourceWord)
                {
                    // 同音語チェックを最適化
                    bool isDuplicate = false;
                    for (const auto &[checkID, checkWord] : target->Words)
                    {
                        if (checkWord.GetForm() == bestSourceWord->GetForm())
                        {
                            isDuplicate = true;
                            break;
                        }
                    }
                    if (!isDuplicate)
                    {
                        tWord.GetForm() = bestSourceWord->GetForm();

                        // ログ
                        const auto dif = LanguageDifference::CreateLoanword(sID, tID, Period, bestSourceWordID, tWordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
            }
        }
    }
}

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param table 音素表
 * @return 音素
 */
Phomene getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table)
{
    // 1. 空ではないセルの「座標」をリストに貯める
    std::vector<Phomene> pool;
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

/**
 * @brief 言語から、音素をランダムに1つ選択する
 * @param language 言語
 * @return 音素
 */
Phomene getRandomSoundFromLanguage(Language &language)
{
    if (language.Words.empty())
    {
        return {0, 0};
    }
    const int index1 = getRandomInt(0, (int)(language.Words.size()) - 1);
    const int index2 = getRandomInt(0, (int)(language.Words[index1].GetForm().size()) - 1);
    return language.Words[index1].GetForm()[index2];
}

/**
 * @brief 言語の影響度をランダムに変化させる
 *
 * @param pChangeStrength 変化率
 */
void LanguageFamilySimulator::ChangeLanguageStrengthRandom(const double pChangeStrength)
{
    for (auto &[ID, language] : Languages)
    {
        if (getWithProbability(pChangeStrength))
        {
            language.Strength = language.Strength * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;

            // ログ
            const auto dif = LanguageDifference::CreateChangeStrength(ID, Period, language.Strength);
            LanguageFamily_.languageDifference.emplace_back(dif);
        }
    }
}

/**
 * @brief 言語からランダムに単語を消去する
 *
 * @param pWordLoss 単語消去率
 */
void LanguageFamilySimulator::ObsoleteWordRandom(const double pWordLoss)
{
    for (auto &[ID, language] : Languages)
    {
        // 単語が脱落するかどうか
        if (getWithProbability(pWordLoss))
        {
            if (language.Words.empty())
                return;

            std::map<std::vector<Phomene>, std::vector<int>> mapReconstructedWordToWordIndice;
            for (const auto &[id, word] : language.Words)
            {
                mapReconstructedWordToWordIndice[word.ReconstructedWord].push_back(id);
            }

            std::vector<int> duplicatedIds;
            for (const auto &[key, ids] : mapReconstructedWordToWordIndice)
            {
                if (ids.size() > 1)
                    duplicatedIds.insert(duplicatedIds.end(), ids.begin(), ids.end());
            }

            if (!duplicatedIds.empty())
            {
                int targetId = duplicatedIds[getRandomInt(0, duplicatedIds.size() - 1)];
                language.Words.erase(targetId); // mapのキー指定削除はO(log N)

                // ログ
                const auto dif = LanguageDifference::CreateObsoleteWord(ID, Period, targetId);
                LanguageFamily_.languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 言語に単語を追加する
 *
 * @param pWordBirth 単語追加律
 */
void LanguageFamilySimulator::MakeCompoundRandom(const double pWordBirth)
{
    for (auto &[ID, language] : Languages)
    {
        // 単語を追加するかどうか
        if (getWithProbability(pWordBirth))
        {
            if (language.Words.empty())
            {
                return;
            }
            const auto wordID1 = getRandomInt(0, (int)language.Words.size() - 1);
            const auto word1 = language.Words[wordID1];
            const auto wordID2 = getRandomInt(0, (int)language.Words.size() - 1);
            const auto word2 = language.Words[wordID2];

            auto newWord = word1.Add(word2);
            newWord.UpdateReconstructedWord(ProtoLanguage);

            const int newWordId = language.Words.rbegin()->first + 1;
            language.Words[newWordId] = newWord;

            // ログ出力
            const auto dif = LanguageDifference::CreateAddCompound(ID, Period, newWordId, {wordID1, wordID2});
            LanguageFamily_.languageDifference.emplace_back(dif);
        }
    }
}

/**
 * @brief 各地に言語があるか
 *
 * @return true
 * @return false
 */
bool LanguageFamilySimulator::HasAllPlaceLanguage()
{
    const auto activePlaces = getNonEmptyStrings(LanguageFamily_.Geography);
    for (const auto &place : activePlaces)
    {
        // find を使うことで「存在チェック」と「データアクセス」を1回で済ませる
        auto it = Languages.find(place);
        if (it == Languages.end() || it->second.Words.empty())
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 時代を進める
 *
 */
void LanguageFamilySimulator::ToNextPeriod()
{
    Period++;
}

/**
 * @brief 差分を適用
 *
 * @param diff 差分
 */
void LanguageFamilySimulator::ApplyDifference(const LanguageDifference &diff)
{
    const auto places = getNonEmptyStrings(LanguageFamily_.Geography);
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);

    switch (diff.Type)
    {
    case LanguageDifferenceType::AddWord:
    {
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]].GetForm() = converter.ConvertToPhoneme(diff.StringParam[1]);
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]].Meanings = diff.SemanticChange;
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        Languages[diff.StringParam[0]].Strength = diff.DoubleParam[0];
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        auto itWord = Languages[diff.StringParam[0]].Words.find(diff.IntParam[0]);
        if (itWord != Languages[diff.StringParam[0]].Words.end())
        {
            // 音韻変化を適用（インプレース更新）
            const auto &sc = diff.PhonologicalChanges;
            std::vector<Phomene> nextSounds;
            nextSounds.reserve(itWord->second.GetForm().size());

            for (size_t i = 0; i < itWord->second.GetForm().size(); ++i)
            {
                bool isMatch = (itWord->second.GetForm()[i] == sc.BeforePhoneme);
                if (isMatch)
                {
                    if (sc.PhoneticEnvironment == PhoneticEnvironment::Start && i != 0)
                        isMatch = false;
                    else if (sc.PhoneticEnvironment == PhoneticEnvironment::End && i != itWord->second.GetForm().size() - 1)
                        isMatch = false;
                    else if (sc.PhoneticEnvironment == PhoneticEnvironment::Middle && (i == 0 || i == itWord->second.GetForm().size() - 1))
                        isMatch = false;
                }
                if (isMatch)
                {
                    if (!sc.IsRemove)
                        nextSounds.push_back(sc.AfterPhoneme);
                }
                else
                {
                    nextSounds.push_back(itWord->second.GetForm()[i]);
                }
            }
            itWord->second.GetForm() = std::move(nextSounds);
        }
        break;
    }

    case LanguageDifferenceType::SemanticChange:
    {
        auto itWord = Languages[diff.StringParam[0]].Words.find(diff.IntParam[0]);
        if (itWord != Languages[diff.StringParam[0]].Words.end())
        {
            itWord->second.Meanings = diff.SemanticChange;
            itWord->second.UpdateReconstructedWord(ProtoLanguage);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        if (Languages.count(diff.StringParam[0]) == 1)
        {
            const auto referenceLanguage = Languages.at(diff.StringParam[0]);
            if (referenceLanguage.Words.count(diff.IntParam[0]) == 1)
            {
                const auto referenceWord = referenceLanguage.Words.at(diff.IntParam[0]);
                Languages[diff.StringParam[1]].Words[diff.IntParam[1]] = referenceWord;
            }
        }
    }

    case LanguageDifferenceType::AddCompound:
    {
        WordForSimulation newWord;
        bool first = true;
        // IntParam[2]以降に合成元の単語IDリストが格納されている
        for (size_t i = 1; i < diff.IntParam.size(); ++i)
        {
            auto itPart = Languages[diff.StringParam[0]].Words.find(diff.IntParam[i]);
            if (itPart != Languages[diff.StringParam[0]].Words.end())
            {
                if (first)
                {
                    newWord = itPart->second;
                    first = false;
                }
                else
                    newWord = newWord.Add(itPart->second);
            }
        }
        newWord.UpdateReconstructedWord(ProtoLanguage);
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]] = std::move(newWord);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        Languages[diff.StringParam[0]].Words.erase(diff.IntParam[0]);
        break;
    }
    }
}

/**
 * @brief 差分を複数適用
 *
 * @param diffs 差分
 */
void LanguageFamilySimulator::ApplyDifferences(const std::vector<LanguageDifference> &diffs)
{
    for (const auto &diff : diffs)
    {
        ApplyDifference(diff);
    }
}

/**
 * @brief 語族をセットする。差分情報の適用も同時にしておく
 *
 * @param languageFamily 語族
 */
void LanguageFamilySimulator::SetLanguageFamily(LanguageFamily languageFamily)
{
    LanguageFamily_ = languageFamily;
    Period = 0;
    Languages.clear();
    ProtoLanguage = Language();

    ApplyDifferences(languageFamily.languageDifference);
}
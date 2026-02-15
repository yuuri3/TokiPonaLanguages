#include "LanguageFamily.h"
#include <fstream>
#include <set>
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

    // ヘルパー関数：ベクトルをカンマ区切りのリスト形式にする
    template <typename T>
    std::string formatYamlList(const std::vector<T> &vec)
    {
        if (vec.empty())
            return "[]";
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ss << vec[i] << (i == vec.size() - 1 ? "" : ", ");
        }
        ss << "]";
        return ss.str();
    }

    // 前後のスペースを除去。 スペースのみのセルは空文字列とする。
    std::string EraseSpace(std::string str)
    {
        size_t first = str.find_first_not_of(" ");
        size_t last = str.find_last_not_of(" ");

        if (first != std::string::npos)
            return str.substr(first, (last - first + 1));
        else
            return "";
    }

    // YAMLの [a, b, c] 形式を vector<string> に変換する
    std::vector<std::string> parseYamlList(const std::string &line)
    {
        std::vector<std::string> result;
        size_t start = line.find('[');
        size_t end = line.find(']');
        if (start == std::string::npos || end == std::string::npos)
            return result;

        std::string content = line.substr(start + 1, end - start - 1);
        std::stringstream ss(content);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            result.push_back(EraseSpace(item));
        }
        return result;
    }

    /**
     * @brief 文字列を':'で前後2つに分割する
     * @return {前部分, 後部分}。コロンがない場合は {元の文字列, ""}
     */
    std::pair<std::string, std::string> splitByColon(const std::string &line)
    {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
        {
            return {line, ""};
        }

        std::string first = line.substr(0, pos);
        std::string second = line.substr(pos + 1);

        // 必要に応じて前後の空白を削除（トリミング）
        auto trim = [](std::string &s)
        {
            size_t f = s.find_first_not_of(" ");
            size_t l = s.find_last_not_of(" \r\n\t");
            if (f == std::string::npos)
                s = "";
            else
                s = s.substr(f, l - f + 1);
        };

        trim(first);
        trim(second);

        return {first, second};
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
void LanguageFamily::SetProtoLanguageOnGeography(
    const std::string &startPlace,
    const Language &protoLanguage)
{
    Languages = setProtoLanguageOnMap(getNonEmptyStrings(Geography), startPlace, protoLanguage);
    ProtoLanguage = protoLanguage;

    // ログ
    PhonemeConverter converter = PhonemeConverter::Create(PhonemeTable);
    languageDifference.emplace_back(LanguageDifference::CreateChangeStrength(startPlace, Period, protoLanguage.Strength));
    for (const auto &[ID, word] : protoLanguage.Words)
    {
        languageDifference.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period, ID, converter.ConvertToString(word.Form)));
    }
}

/**
 * @brief Get the 単語 object
 *
 * @param place 位置
 * @return std::vector<std::string>
 */
std::vector<std::string> LanguageFamily::GetWords(std::string place)
{
    if (Languages.count(place) == 0)
    {
        return {};
    }
    const auto language = Languages[place];
    std::vector<std::string> result;
    PhonemeConverter converter = PhonemeConverter::Create(PhonemeTable);
    for (const auto &[_, word] : language.Words)
    {
        result.emplace_back(converter.ConvertToString(word.Form));
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
void LanguageFamily::PhonologicalChangeRandom(
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
        PhonologicalChange phonologicalChange = makepPhonologicalChangeRandom(sound, PhonemeTable, pSoundLoss);
        // changeLanguageSound(language, soundChange, isProhibitMinimalPair, isSoundDuplication);
        {
            // 子音と母音の境界（定数化してループ外で定義）
            constexpr int MAX_CONSONANT_MANNER = 3;

            // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
            std::map<int, Word> updatedWords;

            // 1. 音韻変化の適用と音素重複チェックを同時に行う
            for (auto &[wordID, word] : language.Words)
            {
                bool changed = false;
                std::vector<Phomene> nextSounds;
                nextSounds.reserve(word.Form.size()); // メモリ確保を1回に抑制

                for (size_t i = 0; i < word.Form.size(); ++i)
                {
                    const auto &currentPhon = word.Form[i];

                    // 変化条件の判定
                    bool isMatch = (currentPhon == phonologicalChange.BeforePhoneme);
                    if (isMatch)
                    {
                        if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Start && i != 0)
                            isMatch = false;
                        else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::End && i != word.Form.size() - 1)
                            isMatch = false;
                        else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Middle && (i == 0 || i == word.Form.size() - 1))
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
                Word newWord = word;
                newWord.Form = std::move(nextSounds); // 所有権を移転してコピーを回避
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
                    soundCounts[it != updatedWords.end() ? it->second.Form : word.Form]++;
                }

                // 重複が発生する変化を差し止める
                for (auto it = updatedWords.begin(); it != updatedWords.end();)
                {
                    if (soundCounts[it->second.Form] > 1)
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
                languageDifference.emplace_back(dif);
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
void LanguageFamily::SemanticChangeRandom(
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
            Word &targetWord = it->second;

            // 変化の種となる単語をもう一つ選択
            int seedIdx = getRandomInt(0, (int)language.Words.size() - 1);
            auto itSeed = language.Words.begin();
            std::advance(itSeed, seedIdx);
            const Word &seedWord = itSeed->second;

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
                languageDifference.emplace_back(dif);
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
    std::vector<std::map<std::vector<Phomene>, std::vector<const Word *>>> mapsReconstructedWordToWord;
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
        std::string s = getCachedString(word.Form);
        if (s == "toki")
            indexToki = id;
        if (s == "pona")
            indexPona = id;
    }

    file << "Toki Pona,";
    if (indexToki != -1 && indexPona != -1)
    {
        const auto &tokiSounds = protoLanguage.Words[indexToki].Form;
        const auto &ponaSounds = protoLanguage.Words[indexPona].Form;

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
                std::string tokiStr = getCachedString(tokiList[0]->Form);
                std::string ponaStr = getCachedString(ponaList[0]->Form);
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
        const auto &protoSounds = reconstructedWord.Form;

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
                    file << getCachedString(derivedWords[row]->Form);
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
void LanguageFamily::ExportLanguageToCSV(const std::string &filename)
{
    exportLanguageToCSV(ProtoLanguage, Languages, PhonemeTable, filename);
}

/**
 * 単語を借用
 *
 * @param nLoanword 借用回数
 * @param pLoanword 借用率
 *
 * @note 借用の履歴をlanguageに記録
 */
void LanguageFamily::LoanwordRandom(const int nLoanword, const double pLoanword)
{
    const auto mapAdjacentData = getAdjacencies(Geography);
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
            const auto sID = (l1.Strength > l2.Strength) ? it1->first : it2->first;
            const auto tID = (l1.Strength > l2.Strength) ? it2->first : it1->first;

            for (auto &[tWordID, tWord] : target->Words)
            {
                if (getRandomInt(0, 1) != 0)
                    continue;

                const Word *bestSourceWord = nullptr;
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
                        if (checkWord.Form == bestSourceWord->Form)
                        {
                            isDuplicate = true;
                            break;
                        }
                    }
                    if (!isDuplicate)
                    {
                        tWord.Form = bestSourceWord->Form;

                        // ログ
                        const auto dif = LanguageDifference::CreateLoanword(sID, tID, Period, bestSourceWordID, tWordID);
                        languageDifference.emplace_back(dif);
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
    const int index2 = getRandomInt(0, (int)(language.Words[index1].Form.size()) - 1);
    return language.Words[index1].Form[index2];
}

/**
 * @brief 言語の影響度をランダムに変化させる
 *
 * @param pChangeStrength 変化率
 */
void LanguageFamily::ChangeLanguageStrengthRandom(const double pChangeStrength)
{
    for (auto &[ID, language] : Languages)
    {
        if (getWithProbability(pChangeStrength))
        {
            language.Strength = language.Strength * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;

            // ログ
            const auto dif = LanguageDifference::CreateChangeStrength(ID, Period, language.Strength);
            languageDifference.emplace_back(dif);
        }
    }
}

/**
 * @brief 言語からランダムに単語を消去する
 *
 * @param pWordLoss 単語消去率
 */
void LanguageFamily::ObsoleteWordRandom(const double pWordLoss)
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
                languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 言語に単語を追加する
 *
 * @param pWordBirth 単語追加律
 */
void LanguageFamily::MakeCompoundRandom(const double pWordBirth)
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
            languageDifference.emplace_back(dif);
        }
    }
}

/**
 * @brief 各地に言語があるか
 *
 * @return true
 * @return false
 */
bool LanguageFamily::HasAllPlaceLanguage()
{
    const auto activePlaces = getNonEmptyStrings(Geography);
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
void LanguageFamily::ToNextPeriod()
{
    Period++;
}

/**
 * @brief 差分を適用
 *
 * @param diff 差分
 */
void LanguageFamily::ApplyDifference(const LanguageDifference &diff)
{
    const auto places = getNonEmptyStrings(Geography);
    PhonemeConverter converter = PhonemeConverter::Create(PhonemeTable);

    switch (diff.Type)
    {
    case LanguageDifferenceType::AddWord:
    {
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]].Form = converter.ConvertToPhoneme(diff.StringParam[1]);
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
            nextSounds.reserve(itWord->second.Form.size());

            for (size_t i = 0; i < itWord->second.Form.size(); ++i)
            {
                bool isMatch = (itWord->second.Form[i] == sc.BeforePhoneme);
                if (isMatch)
                {
                    if (sc.PhoneticEnvironment == PhoneticEnvironment::Start && i != 0)
                        isMatch = false;
                    else if (sc.PhoneticEnvironment == PhoneticEnvironment::End && i != itWord->second.Form.size() - 1)
                        isMatch = false;
                    else if (sc.PhoneticEnvironment == PhoneticEnvironment::Middle && (i == 0 || i == itWord->second.Form.size() - 1))
                        isMatch = false;
                }
                if (isMatch)
                {
                    if (!sc.IsRemove)
                        nextSounds.push_back(sc.AfterPhoneme);
                }
                else
                {
                    nextSounds.push_back(itWord->second.Form[i]);
                }
            }
            itWord->second.Form = std::move(nextSounds);
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
        {
            auto itSrc = Languages[diff.StringParam[0]].Words.find(diff.IntParam[0]);
            auto itDst = Languages[diff.StringParam[1]].Words.find(diff.IntParam[1]);
            if (itSrc != Languages[diff.StringParam[0]].Words.end() && itDst != Languages[diff.StringParam[1]].Words.end())
            {
                // 借用：音素列をコピー
                itDst->second.Form = itSrc->second.Form;
            }
            break;
        }
    }

    case LanguageDifferenceType::AddCompound:
    {
        Word newWord;
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
void LanguageFamily::ApplyDifferences(const std::vector<LanguageDifference> &diffs)
{
    for (const auto &diff : diffs)
    {
        ApplyDifference(diff);
    }
}

/**
 * @brief 差分をファイル出力
 *
 */
void LanguageFamily::Export(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return;

    // 2. Map
    file << "Map:\n";
    for (const auto &row : Geography)
    {
        file << "  - [";
        for (size_t i = 0; i < row.size(); ++i)
        {
            file << row[i] << (i == row.size() - 1 ? "" : ", ");
        }
        file << "]\n";
    }

    // 3. PhoneticsMap
    file << "PhoneticsMap:\n";
    for (const auto &row : PhonemeTable)
    {
        file << "  - [";
        for (size_t i = 0; i < row.size(); ++i)
        {
            file << row[i] << (i == row.size() - 1 ? "" : ", ");
        }
        file << "]\n";
    }

    // 4. LanguageDifferences
    file << "LanguageDifferences:\n";
    for (const auto &diff : languageDifference)
    {
        file << "  - Section: " << diff.Period << "\n";
        file << "    Type: " << static_cast<int>(diff.Type) << "\n";

        file << "    IntParam:\n";
        for (const auto &i : diff.IntParam)
            file << "      - " << i << "\n";

        file << "    DoubleParam:\n";
        for (const auto &d : diff.DoubleParam)
            file << "      - " << d << "\n";

        file << "    StringParam:\n";
        for (const auto &s : diff.StringParam)
            file << "      - " << s << "\n";

        file << "    SoundChange:\n";
        file << "      Before:\n";
        file << "        Place: " << diff.PhonologicalChanges.BeforePhoneme.Place << "\n";
        file << "        Mannar: " << diff.PhonologicalChanges.BeforePhoneme.Manner << "\n";
        file << "      After:\n";
        file << "        Place: " << diff.PhonologicalChanges.AfterPhoneme.Place << "\n";
        file << "        Mannar: " << diff.PhonologicalChanges.AfterPhoneme.Manner << "\n";
        file << "      Condition: " << static_cast<int>(diff.PhonologicalChanges.PhoneticEnvironment) << "\n";
        file << "      IsRemove: " << diff.PhonologicalChanges.IsRemove << "\n";

        file << "    MeaningChange:\n";
        for (const auto &pair : diff.SemanticChange)
        {
            file << "      - Key: " << pair.first << "\n";
            file << "        Value: " << pair.second << "\n";
        }
    }
}

/**
 * @brief ファイル読み込み
 *
 * @param filename ファイルパス
 */
void LanguageFamily::Import(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    enum Mode
    {
        Map_,
        PhonemeTable_,
        LanguageDifferences_,
    };

    enum SubMode
    {
        Type_,
        Period_,
        IntParam_,
        DoubleParam_,
        StringParam_,
        PhonologicalChanges_,
        SemanticChange_,
    };

    Mode mode;
    SubMode subMode;
    LanguageDifference dif;
    bool b = false;

    std::string line;
    while (std::getline(file, line))
    {
        if (line == "Map:")
        {
            mode = Mode::Map_;
            Geography.clear();
            continue;
        }
        else if (line == "PhoneticsMap:")
        {
            mode = Mode::PhonemeTable_;
            PhonemeTable.clear();
            continue;
        }
        else if (line == "LanguageDifferences:")
        {
            mode = Mode::LanguageDifferences_;
            languageDifference.clear();
            continue;
        }

        if (mode == Mode::Map_)
        {
            Geography.emplace_back(parseYamlList(line));
        }
        else if (mode == Mode::PhonemeTable_)
        {
            PhonemeTable.emplace_back(parseYamlList(line));
        }
        else if (mode == Mode::LanguageDifferences_)
        {
            auto [key, value] = splitByColon(line);
            if (key == "- Section")
            {
                if (b)
                {
                    languageDifference.emplace_back(dif);
                    dif = LanguageDifference();
                }
                else
                {
                    b = true;
                }
                dif.Period = std::stoi(value);
                continue;
            }
            else if (key == "Type")
            {
                dif.Type = static_cast<LanguageDifferenceType>(std::stoi(value));
                continue;
            }
            else if (line == "    IntParam:")
            {
                subMode = SubMode::IntParam_;
                continue;
            }
            else if (line == "    DoubleParam:")
            {
                subMode = SubMode::DoubleParam_;
                continue;
            }
            else if (line == "    StringParam:")
            {
                subMode = SubMode::StringParam_;
                continue;
            }
            else if (line == "    SoundChange:")
            {
                std::getline(file, line); // Before:
                std::getline(file, line); // Place:
                std::tie(key, value) = splitByColon(line);
                const int beforePlace = std::stoi(value);
                std::getline(file, line); // Manner:
                std::tie(key, value) = splitByColon(line);
                const int beforeManner = std::stoi(value);

                std::getline(file, line); // After:
                std::getline(file, line); // Place:
                std::tie(key, value) = splitByColon(line);
                const int afterPlace = std::stoi(value);
                std::getline(file, line); // Manner:
                std::tie(key, value) = splitByColon(line);
                const int afterManner = std::stoi(value);

                std::getline(file, line); // Condition:
                std::tie(key, value) = splitByColon(line);
                const auto phoneticEnvironment = static_cast<PhoneticEnvironment>(std::stoi(value));

                std::getline(file, line); // IsRemove:
                std::tie(key, value) = splitByColon(line);
                const bool isRemove = static_cast<bool>(std::stoi(value));

                dif.PhonologicalChanges.BeforePhoneme.Place = beforePlace;
                dif.PhonologicalChanges.BeforePhoneme.Manner = beforeManner;
                dif.PhonologicalChanges.AfterPhoneme.Place = afterPlace;
                dif.PhonologicalChanges.AfterPhoneme.Manner = afterManner;
                dif.PhonologicalChanges.PhoneticEnvironment = phoneticEnvironment;
                dif.PhonologicalChanges.IsRemove = isRemove;
                continue;
            }
            else if (line == "    MeaningChange:")
            {
                subMode = SubMode::SemanticChange_;
                continue;
            }

            if (subMode == SubMode::IntParam_)
            {
                dif.IntParam.emplace_back(std::stoi(line.substr(8)));
            }
            else if (subMode == SubMode::DoubleParam_)
            {
                dif.DoubleParam.emplace_back(std::stod(line.substr(8)));
            }
            else if (subMode == SubMode::StringParam_)
            {
                dif.StringParam.emplace_back(line.substr(8));
            }
            else if (subMode == SubMode::SemanticChange_)
            {
                std::getline(file, line);
                const auto [_1, key2] = splitByColon(line);
                std::getline(file, line);
                const auto [_2, value2] = splitByColon(line);
                dif.SemanticChange[key2] = std::stod(value2);
            }
        }
    }
    languageDifference.emplace_back(dif);
}
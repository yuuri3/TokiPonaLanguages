#include "..\\include\LanguageFamilySimulator.h"
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace
{

    // ヘルパー：vectorの中身を文字列に変換
    template <typename T>
    std::string joinVector(const std::vector<T> &vec, const std::string &delimiter = " ")
    {
        std::stringstream ss;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ss << vec[i] << (i == vec.size() - 1 ? "" : delimiter);
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
    const std::vector<std::string> &geometryData,
    const std::string &startPlace,
    const Language &protoLanguage)
{
    std::map<std::string, Language> languageOnGeometry;

    for (const std::string &place : geometryData)
    {
        // 1番目の要素をitem、2番目を空文字で初期化してペアを作成
        Language language;
        language.Strength = 0.0;
        language.Words = {};

        // もし1番目の要素がtargetAと一致したら、2番目をreplacementBにする
        if (place == startPlace)
        {
            language = protoLanguage;
        }

        // 位置を設定
        languageOnGeometry[place] = language;
    }

    return languageOnGeometry;
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
    for (const auto &[wordID, word] : protoLanguage.Words)
    {
        LanguageFamily_.languageDifference.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period, wordID, converter.ConvertToString(word.Form)));
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
    std::vector<std::string> words;
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);
    for (const auto &[_, word] : language.Words)
    {
        words.emplace_back(converter.ConvertToString(word.Form));
    }
    return words;
}

/**
 * @brief 音韻変化を言語に適用
 *
 * @param language 言語
 * @param phonologicalChange 音韻変化
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 * @param isSoundDuplication 音素の重複を禁止するか
 */
void ApplyPhonologicalChange(Language &language, const PhonologicalChange &phonologicalChange, const bool isProhibitSoundDuplication, const bool isProhibitMinimalPair)
{
    // 子音と母音の境界（定数化してループ外で定義）
    constexpr int MAX_CONSONANT_MANNER = 3;

    // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
    std::map<int, Word> phonologicalChangedWords;

    // 1. 音韻変化の適用と音素重複チェックを同時に行う
    for (auto &[wordID, word] : language.Words)
    {
        bool isChanged = false;
        std::vector<Phoneme> changedWordForm;
        changedWordForm.reserve(word.Form.size());

        for (size_t soundPosition = 0; soundPosition < word.Form.size(); ++soundPosition)
        {
            const auto &sound = word.Form[soundPosition];

            // 変化条件の判定
            bool isSoundEqualToBeforePhoneme = (sound == phonologicalChange.BeforePhoneme);
            if (isSoundEqualToBeforePhoneme)
            {
                if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Start && !(soundPosition == 0 || word.Form[soundPosition - 1].IsSpace))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::End && !(soundPosition == word.Form.size() - 1 || word.Form[soundPosition + 1].IsSpace))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Middle && (soundPosition == 0 || word.Form[soundPosition - 1].IsSpace || soundPosition == word.Form.size() - 1 || word.Form[soundPosition + 1].IsSpace))
                    isSoundEqualToBeforePhoneme = false;
            }

            if (isSoundEqualToBeforePhoneme)
            {
                isChanged = true;
                if (!phonologicalChange.IsRemove)
                {
                    changedWordForm.push_back(phonologicalChange.AfterPhoneme);
                }
            }
            else
            {
                changedWordForm.push_back(sound);
            }
        }

        if (!isChanged)
            continue;

        // 子音・母音の重複禁止チェック (isSoundDuplication)
        if (isProhibitSoundDuplication)
        {
            bool isSoundDuplication = false;
            if (changedWordForm.empty())
                isSoundDuplication = true;
            else if (changedWordForm.size() == 1)
            {
                if (changedWordForm[0].Manner <= MAX_CONSONANT_MANNER)
                    isSoundDuplication = true;
            }
            else
            {
                // 境界条件のチェック
                if ((changedWordForm[0].Manner <= MAX_CONSONANT_MANNER && changedWordForm[1].Manner <= MAX_CONSONANT_MANNER) ||
                    (changedWordForm.back().Manner <= MAX_CONSONANT_MANNER && changedWordForm[changedWordForm.size() - 2].Manner <= MAX_CONSONANT_MANNER))
                {
                    isSoundDuplication = true;
                }
                else
                {
                    // 3連続のチェック
                    for (size_t j = 0; j + 2 < changedWordForm.size(); ++j)
                    {
                        bool isConsonant = (changedWordForm[j].Manner <= MAX_CONSONANT_MANNER &&
                                            changedWordForm[j + 1].Manner <= MAX_CONSONANT_MANNER &&
                                            changedWordForm[j + 2].Manner <= MAX_CONSONANT_MANNER);
                        bool isVowel = (changedWordForm[j].Manner > MAX_CONSONANT_MANNER &&
                                        changedWordForm[j + 1].Manner > MAX_CONSONANT_MANNER &&
                                        changedWordForm[j + 2].Manner > MAX_CONSONANT_MANNER);
                        if (isConsonant || isVowel)
                        {
                            isSoundDuplication = true;
                            break;
                        }
                    }
                }
            }
            if (isSoundDuplication)
                continue; // 違反していればこの単語の変化は破棄
        }

        // 変化後の単語候補を一時保存
        Word changedWord = word;
        changedWord.Form = std::move(changedWordForm); // 所有権を移転してコピーを回避
        phonologicalChangedWords[wordID] = std::move(changedWord);
    }

    // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
    if (isProhibitMinimalPair)
    {
        // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
        std::map<std::vector<Phoneme>, int> mimimalPairCount;
        for (const auto &[wordID, word] : language.Words)
        {
            auto it = phonologicalChangedWords.find(wordID);
            mimimalPairCount[it != phonologicalChangedWords.end() ? it->second.Form : word.Form]++;
        }

        // 重複が発生する変化を差し止める
        for (auto it = phonologicalChangedWords.begin(); it != phonologicalChangedWords.end();)
        {
            if (mimimalPairCount[it->second.Form] > 1)
                it = phonologicalChangedWords.erase(it);
            else
                ++it;
        }
    }

    // 3. 最終的な反映（一括代入）
    for (auto &[wordID, phonologicalChangedWord] : phonologicalChangedWords)
    {
        language.Words[wordID] = std::move(phonologicalChangedWord);
    }
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
    const bool isProhibitSoundDuplication)
{
    for (auto &[place, language] : Languages)
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
        const auto randomSound = getRandomSoundFromTable(LanguageFamily_.PhonemeTable);
        PhonologicalChange randomPhonologicalChange = makepPhonologicalChangeRandom(randomSound, LanguageFamily_.PhonemeTable, pSoundLoss);

        // ログ
        const auto dif = LanguageDifference::CreatePhonologicalChange(place, Period, randomPhonologicalChange);
        LanguageFamily_.languageDifference.emplace_back(dif);

        ApplyPhonologicalChange(language, randomPhonologicalChange, isProhibitMinimalPair, isProhibitSoundDuplication);
    }
}

/**
 * 変化規則をランダムに生成
 * @param beforePlace 変化前音素
 * @param beforeMannar 変化前音素
 * @param table 音素表
 * @param pRemovePhoneme 音が脱落する確率
 */
PhonologicalChange makepPhonologicalChangeRandom(const Phoneme &beforePhon, const std::vector<std::vector<std::string>> &phonemeTable, const double pRemoveSound)
{
    int randomPhoneticEnvironment = getRandomInt(0, 2);

    PhonologicalChange randomPhonologicalChange;
    randomPhonologicalChange.BeforePhoneme = beforePhon;
    randomPhonologicalChange.IsRemove = getWithProbability(pRemoveSound);
    switch (randomPhoneticEnvironment)
    {
    case 0:
        randomPhonologicalChange.PhoneticEnvironment = PhoneticEnvironment::Start;
        break;
    case 1:
        randomPhonologicalChange.PhoneticEnvironment = PhoneticEnvironment::Middle;
        break;
    case 2:
        randomPhonologicalChange.PhoneticEnvironment = PhoneticEnvironment::End;
        break;

    default:
        break;
    }
    randomPhonologicalChange.AfterPhoneme = beforePhon;
    moveRandomOnTable(randomPhonologicalChange.AfterPhoneme.Manner, randomPhonologicalChange.AfterPhoneme.Place, phonemeTable);
    return randomPhonologicalChange;
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
    const auto geographyAdjacentData = getAdjacencies(LanguageFamily_.Geography);
    for (int i = 0; i < nLoanword; i++)
    {
        // 借用率 は現在固定
        const auto adjucent = geographyAdjacentData[getRandomInt(0, geographyAdjacentData.size() - 1)];
        {
            auto languageIterator1 = Languages.find(adjucent.first);
            auto languageIterator2 = Languages.find(adjucent.second);
            if (languageIterator1 == Languages.end() || languageIterator2 == Languages.end())
                return;

            Language &language1 = languageIterator1->second;
            Language &language2 = languageIterator2->second;

            if (language1.Words.empty() || language2.Words.empty())
            {
                if (language1.Words.empty())
                {
                    language1.Words = language2.Words;
                    language1.Strength = language2.Strength;

                    // ログ
                    for (const auto &[wordID, word] : language2.Words)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(languageIterator2->first, languageIterator1->first, Period, wordID, wordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
                else
                {
                    language2.Words = language1.Words;
                    language2.Strength = language1.Strength;

                    // ログ
                    for (const auto &[wordID, word] : language1.Words)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(languageIterator1->first, languageIterator2->first, Period, wordID, wordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
                return;
            }

            auto *referenceLanguage = (language1.Strength > language2.Strength) ? &language1 : &language2;
            auto *targetLanguage = (language1.Strength > language2.Strength) ? &language2 : &language1;
            const auto referencePlace = (language1.Strength > language2.Strength) ? languageIterator1->first : languageIterator2->first;
            const auto targetPlace = (language1.Strength > language2.Strength) ? languageIterator2->first : languageIterator1->first;

            for (auto &[targetWordID, targetWord] : targetLanguage->Words)
            {
                // 借用率は 0.5 で固定
                if (getRandomInt(0, 1) != 0)
                    continue;

                targetWord = referenceLanguage->Words[targetWordID];

                // ログ
                const auto dif = LanguageDifference::CreateLoanword(referencePlace, targetPlace, Period, targetWordID, targetWordID);
                LanguageFamily_.languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param phonemeTable 音素表
 * @return 音素
 */
Phoneme getRandomSoundFromTable(const std::vector<std::vector<std::string>> &phonemeTable)
{
    // 1. 空ではないセルの「座標」をリストに貯める
    std::vector<Phoneme> phonemeLiist;
    phonemeLiist.reserve(phonemeTable.size() * phonemeTable[0].size());

    for (int row = 0; row < (int)phonemeTable.size(); ++row)
    {
        for (int collum = 0; collum < (int)phonemeTable[row].size(); ++collum)
        {
            if (!phonemeTable[row][collum].empty())
            {
                Phoneme phoneme;
                phoneme.IsSpace = false;
                phoneme.Manner = row;
                phoneme.Place = collum;
                phonemeLiist.push_back({phoneme});
            }
        }
    }

    // 候補が一つもない場合
    if (phonemeLiist.empty())
    {
        Phoneme phoneme;
        phoneme.IsSpace = true;
        phoneme.Manner = -1;
        phoneme.Place = -1;
        return phoneme;
    }

    // 座標リストのインデックスをランダムに選択
    return phonemeLiist[getRandomInt(0, phonemeLiist.size() - 1)];
}

/**
 * @brief 言語の影響度をランダムに変化させる
 *
 * @param pChangeStrength 変化率
 */
void LanguageFamilySimulator::ChangeLanguageStrengthRandom(const double pChangeStrength)
{
    for (auto &[place, language] : Languages)
    {
        if (getWithProbability(pChangeStrength))
        {
            language.Strength = language.Strength * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;

            // ログ
            const auto dif = LanguageDifference::CreateChangeStrength(place, Period, language.Strength);
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
    const auto places = getNonEmptyStrings(LanguageFamily_.Geography);
    for (const auto &place : places)
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
 *
 * @return 成否
 */
bool LanguageFamilySimulator::ApplyDifference(const LanguageDifference &diff)
{
    const auto places = getNonEmptyStrings(LanguageFamily_.Geography);
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);

    switch (diff.Type)
    {
    case LanguageDifferenceType::AddWord:
    {
        if (diff.IntParam.size() < 1 || diff.StringParam.size() < 2)
        {
            return false;
        }

        const auto geometry = diff.StringParam[0];
        const auto wordID = diff.IntParam[0];
        const auto form = diff.StringParam[1];

        Languages[geometry].Words[wordID].Form = converter.ConvertToPhoneme(form);
        if (diff.Period == 0 && geometry == "0")
        {
            ProtoLanguage.Words[wordID].Form = converter.ConvertToPhoneme(form);
        }
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        if (diff.DoubleParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam[0];
        const auto strength = diff.DoubleParam[0];

        Languages[geometry].Strength = strength;
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        if (diff.StringParam.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam[0];

        if (Languages.count(geometry) == 1)
        {
            ApplyPhonologicalChange(Languages[geometry], diff.PhonologicalChanges, true, true);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        if (diff.IntParam.size() < 2 || diff.StringParam.size() < 2)
        {
            return false;
        }

        const auto referenceGeometry = diff.StringParam[0];
        const auto targetGeometry = diff.StringParam[1];
        const auto referenceWordID = diff.IntParam[0];
        const auto targetWordID = diff.IntParam[1];

        if (Languages.count(referenceGeometry) == 1)
        {
            const auto referenceLanguage = Languages.at(referenceGeometry);
            if (referenceLanguage.Words.count(referenceWordID) == 1)
            {
                const auto referenceWord = referenceLanguage.Words.at(referenceWordID);
                Languages[targetGeometry].Words[targetWordID] = referenceWord;
            }
        }
        break;
    }

    case LanguageDifferenceType::AddCompound:
    {
        if (diff.IntParam.size() < 2 || diff.StringParam.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam[0];
        const auto wordID = diff.IntParam[0];

        Word compound;
        // IntParam[1]以降に合成元の単語IDリストが格納されている
        for (size_t i = 1; i < diff.IntParam.size(); ++i)
        {
            auto itPart = Languages[geometry].Words.find(diff.IntParam[i]);
            if (itPart != Languages[geometry].Words.end())
            {
                compound = compound.Add(itPart->second);
            }
        }
        Languages[geometry].Words[wordID] = std::move(compound);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        if (diff.IntParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam[0];
        const auto wordID = diff.IntParam[0];

        Languages[geometry].Words.erase(wordID);
        break;
    }

    default:
    {
        return false;
    }
    }
    return true;
}

/**
 * @brief 差分を複数適用
 *
 * @param diffs 差分
 *
 * @return 成否
 */
bool LanguageFamilySimulator::ApplyDifferences(const std::vector<LanguageDifference> &diffs)
{
    for (const auto &diff : diffs)
    {
        if (!ApplyDifference(diff))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief インスタンス生成
 *
 */
std::optional<LanguageFamilySimulator> LanguageFamilySimulator::Create()
{
    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_.languageDifference = {};
    simulator.LanguageFamily_.Geography = {{""}};
    simulator.LanguageFamily_.PhonemeTable = {{""}};
    simulator.Period = 0;
    simulator.Languages.clear();
    simulator.ProtoLanguage = Language();

    return simulator;
}

/**
 * @brief インスタンス生成
 *
 * @param languageFamily 語族
 */
std::optional<LanguageFamilySimulator> LanguageFamilySimulator::Create(LanguageFamily languageFamily)
{
    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_ = languageFamily;
    simulator.Period = 0;
    simulator.Languages.clear();
    simulator.ProtoLanguage = Language();

    if (!simulator.ApplyDifferences(languageFamily.languageDifference))
    {
        return std::nullopt;
    }
    return simulator;
}

/**
 * @brief 言語名の配列を出力
 *
 * @return std::vector<std::vector<std::string>>
 */
std::vector<std::vector<std::string>> LanguageFamilySimulator::ToStringLanguageFamily()
{
    int currentPeriod = 0;
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;
    Period = 0;
    Languages.clear();

    auto converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography))
    {
        line.emplace_back(place);
    }
    result.emplace_back(line);
    line.clear();

    for (const auto &diff : LanguageFamily_.languageDifference)
    {
        if (diff.Period != currentPeriod)
        {
            currentPeriod = diff.Period;
            for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography))
            {
                if (Languages.count(place) == 0 || Languages[place].Words.empty())
                {
                    line.emplace_back("");
                }
                else
                {
                    line.emplace_back(converter.ConvertToString(Languages[place].Words[0].Form));
                }
            }
            result.emplace_back(line);
            line.clear();
        }
        if (!ApplyDifference(diff))
        {
            return {};
        }
    }

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography))
    {
        if (Languages.count(place) == 0 || Languages[place].Words.empty())
        {
            line.emplace_back("");
        }
        else
        {
            line.emplace_back(converter.ConvertToString(Languages[place].Words[0].Form));
        }
    }
    result.emplace_back(line);
    line.clear();

    return result;
}

/**
 * @brief 文字列の配列に変換
 *
 * @return std::vector<std::vector<std::string>> 配列
 */
std::vector<std::vector<std::string>> LanguageFamilySimulator::ToStringCurrentLanguages()
{
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;

    // 1. 文字列変換の結果をキャッシュするマップ (高速化の肝)
    std::map<std::vector<Phoneme>, std::string> convertCache;
    auto getCachedString = [&](const std::vector<Phoneme> &s) -> const std::string &
    {
        auto iterator = convertCache.find(s);
        if (iterator != convertCache.end())
            return iterator->second;
        PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);
        return convertCache[s] = converter.ConvertToString(s); //
    };

    // 2. ヘッダー行 (Place) の出力と、Languageポインタのキャッシュ
    line.emplace_back("");
    std::vector<const Language *> languagePtrList;
    languagePtrList.reserve(Languages.size());
    for (const auto &[place, language] : Languages)
    {
        line.emplace_back(place);
        languagePtrList.push_back(&language); // ループ内でのmap検索を避けるために保持
    }
    result.emplace_back(line);
    line.clear();

    // 5. 各単語の出力
    for (const auto &[wordID, _] : ProtoLanguage.Words)
    {
        line.emplace_back(getCachedString(ProtoLanguage.Words.at(wordID).Form));
        for (const auto &[__, language] : Languages)
        {
            line.emplace_back(getCachedString(language.Words.at(wordID).Form));
        }
        result.emplace_back(line);
        line.clear();
    }
    return result;
}
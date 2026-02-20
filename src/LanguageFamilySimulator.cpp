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
        LanguageFamily_.languageDifference.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period, wordID, converter.ConvertToString(word.GetForm())));
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
        words.emplace_back(converter.ConvertToString(word.GetForm()));
    }
    return words;
}

/**
 * 音素列を変換表に基づいて文字列に復元する
 * @param Phonemes 音素列
 * @param table 音素表
 */
std::string PhonemeConverter::ConvertToString(const std::vector<Phomene> &phonemes)
{
    std::string str = "";

    for (const auto &phoneme : phonemes)
    {
        for (const auto &[keyString, valuePhoneme] : PhonemeMap)
        {
            if (phoneme == valuePhoneme)
            {
                str += keyString;
            }
        }
    }

    return str;
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
        const auto randomSound = getRandomSoundFromLanguage(language);
        PhonologicalChange randomPhonologicalChange = makepPhonologicalChangeRandom(randomSound, LanguageFamily_.PhonemeTable, pSoundLoss);
        // changeLanguageSound(language, soundChange, isProhibitMinimalPair, isSoundDuplication);
        {
            // 子音と母音の境界（定数化してループ外で定義）
            constexpr int MAX_CONSONANT_MANNER = 3;

            // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
            std::map<int, WordForSimulation> phonologicalChangedWords;

            // 1. 音韻変化の適用と音素重複チェックを同時に行う
            for (auto &[wordID, word] : language.Words)
            {
                bool isChanged = false;
                std::vector<Phomene> changedWordForm;
                changedWordForm.reserve(word.GetForm().size());

                for (size_t soundPosition = 0; soundPosition < word.GetForm().size(); ++soundPosition)
                {
                    const auto &sound = word.GetForm()[soundPosition];

                    // 変化条件の判定
                    bool isSoundEqualToBeforePhoneme = (sound == randomPhonologicalChange.BeforePhoneme);
                    if (isSoundEqualToBeforePhoneme)
                    {
                        if (randomPhonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Start && soundPosition != 0)
                            isSoundEqualToBeforePhoneme = false;
                        else if (randomPhonologicalChange.PhoneticEnvironment == PhoneticEnvironment::End && soundPosition != word.GetForm().size() - 1)
                            isSoundEqualToBeforePhoneme = false;
                        else if (randomPhonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Middle && (soundPosition == 0 || soundPosition == word.GetForm().size() - 1))
                            isSoundEqualToBeforePhoneme = false;
                    }

                    if (isSoundEqualToBeforePhoneme)
                    {
                        isChanged = true;
                        if (!randomPhonologicalChange.IsRemove)
                        {
                            changedWordForm.push_back(randomPhonologicalChange.AfterPhoneme);
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
                WordForSimulation changedWord = word;
                changedWord.GetForm() = std::move(changedWordForm); // 所有権を移転してコピーを回避
                phonologicalChangedWords[wordID] = std::move(changedWord);
            }

            // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
            if (isProhibitMinimalPair)
            {
                // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
                std::map<std::vector<Phomene>, int> mimimalPairCount;
                for (const auto &[wordID, word] : language.Words)
                {
                    auto it = phonologicalChangedWords.find(wordID);
                    mimimalPairCount[it != phonologicalChangedWords.end() ? it->second.GetForm() : word.GetForm()]++;
                }

                // 重複が発生する変化を差し止める
                for (auto it = phonologicalChangedWords.begin(); it != phonologicalChangedWords.end();)
                {
                    if (mimimalPairCount[it->second.GetForm()] > 1)
                        it = phonologicalChangedWords.erase(it);
                    else
                        ++it;
                }
            }

            // 3. 最終的な反映（一括代入）
            for (auto &[wordID, phonologicalChangedWord] : phonologicalChangedWords)
            {
                language.Words[wordID] = std::move(phonologicalChangedWord);

                // ログ
                const auto dif = LanguageDifference::CreatePhonologicalChange(place, Period, wordID, randomPhonologicalChange);
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
    for (auto &[place, language] : Languages)
    {
        // 意味変化するかどうか
        if (getWithProbability(pSemanticShift))
        {
            if (language.Words.empty())
                return;

            // 変更対象の単語をランダムに選択
            // マップの要素にランダムアクセスするため、イテレータを進める
            int targetWordIteratorPosition = getRandomInt(0, (int)language.Words.size() - 1);
            auto targetWordIterator = language.Words.begin();
            std::advance(targetWordIterator, targetWordIteratorPosition);
            const auto wordID = targetWordIterator->first;
            WordForSimulation &targetWord = targetWordIterator->second;

            // 変化の種となる単語をもう一つ選択
            int referenceWordIteratorPosition = getRandomInt(0, (int)language.Words.size() - 1);
            auto referenceWordIterator = language.Words.begin();
            std::advance(referenceWordIterator, referenceWordIteratorPosition);
            const WordForSimulation &referenceWord = referenceWordIterator->second;

            // 現在の状態を保存（ロールバック用）
            Meaning currentTargetMeaning = targetWord.Meanings;
            std::vector<Phomene> currentTargetReconstructedWord = targetWord.ReconstructedWord;

            // 意味の変化を適用
            double changeRate = getRandomDouble(0.0, maxSemanticShiftRate);
            targetWord.Meanings = targetWord.Meanings.Add(referenceWord.Meanings.Product(changeRate));
            targetWord.Meanings.Normalize();
            targetWord.UpdateReconstructedWord(ProtoLanguage);

            // 整合性チェック：すべての単語が異なる祖語に対応しているか
            bool isSurjectiveWordToConstructedWord = false;
            if (targetWord.ReconstructedWord == currentTargetReconstructedWord)
            {
                isSurjectiveWordToConstructedWord = true;
            }
            else
            {
                for (auto iterator = language.Words.begin(); iterator != language.Words.end(); ++iterator)
                {
                    if (iterator == targetWordIterator)
                        continue; // 自分自身はスキップ
                    if (iterator->second.ReconstructedWord == currentTargetReconstructedWord)
                    {
                        isSurjectiveWordToConstructedWord = true;
                        break;
                    }
                }
            }

            // 全射でない場合は元の状態に戻す
            if (!isSurjectiveWordToConstructedWord)
            {
                targetWord.Meanings = std::move(currentTargetMeaning);
                targetWord.ReconstructedWord = std::move(currentTargetReconstructedWord);
            }
            else
            {
                // ログ
                const auto dif = LanguageDifference::CreateSemanticChange(place, Period, wordID, referenceWord.Meanings);
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
PhonologicalChange makepPhonologicalChangeRandom(const Phomene &beforePhon, const std::vector<std::vector<std::string>> &phonemeTable, const double pRemoveSound)
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
                if (getRandomInt(0, 1) != 0)
                    continue;

                const WordForSimulation *referenceWordSameMeaning = nullptr;
                int referenceWordSameMeaningID = -1;
                double maxMeaningDot = -1.0;

                for (const auto &[referenceWordID, referenceWord] : referenceLanguage->Words)
                {
                    double meaningDot = targetWord.Meanings.Dot(referenceWord.Meanings);
                    if (meaningDot > maxMeaningDot)
                    {
                        maxMeaningDot = meaningDot;
                        referenceWordSameMeaning = &referenceWord;
                        referenceWordSameMeaningID = referenceWordID;
                    }
                }

                if (referenceWordSameMeaning)
                {
                    // 同音語チェックを最適化
                    bool isHomophone = false;
                    for (const auto &[targetWordID, targetWord] : targetLanguage->Words)
                    {
                        if (targetWord.GetForm() == referenceWordSameMeaning->GetForm())
                        {
                            isHomophone = true;
                            break;
                        }
                    }
                    if (!isHomophone)
                    {
                        targetWord.GetForm() = referenceWordSameMeaning->GetForm();

                        // ログ
                        const auto dif = LanguageDifference::CreateLoanword(referencePlace, targetPlace, Period, referenceWordSameMeaningID, targetWordID);
                        LanguageFamily_.languageDifference.emplace_back(dif);
                    }
                }
            }
        }
    }
}

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param phonemeTable 音素表
 * @return 音素
 */
Phomene getRandomSoundFromTable(const std::vector<std::vector<std::string>> &phonemeTable)
{
    // 1. 空ではないセルの「座標」をリストに貯める
    std::vector<Phomene> phonemeLiist;
    phonemeLiist.reserve(phonemeTable.size() * phonemeTable[0].size());

    for (int row = 0; row < (int)phonemeTable.size(); ++row)
    {
        for (int collum = 0; collum < (int)phonemeTable[row].size(); ++collum)
        {
            if (!phonemeTable[row][collum].empty())
            {
                phonemeLiist.push_back({row, collum});
            }
        }
    }

    // 候補が一つもない場合
    if (phonemeLiist.empty())
    {
        return {-1, -1};
    }

    // 座標リストのインデックスをランダムに選択
    return phonemeLiist[getRandomInt(0, phonemeLiist.size() - 1)];
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
    const int randomWordIndex = getRandomInt(0, (int)(language.Words.size()) - 1);
    const int PhonemeIndex = getRandomInt(0, (int)(language.Words[randomWordIndex].GetForm().size()) - 1);
    return language.Words[randomWordIndex].GetForm()[PhonemeIndex];
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
 * @brief 言語からランダムに単語を消去する
 *
 * @param pWordLoss 単語消去率
 */
void LanguageFamilySimulator::ObsoleteWordRandom(const double pWordLoss)
{
    for (auto &[place, language] : Languages)
    {
        // 単語が脱落するかどうか
        if (getWithProbability(pWordLoss))
        {
            if (language.Words.empty())
                return;

            std::map<std::vector<Phomene>, std::vector<int>> mapReconstructedWordToWordIndice;
            for (const auto &[wordID, word] : language.Words)
            {
                mapReconstructedWordToWordIndice[word.ReconstructedWord].push_back(wordID);
            }

            std::vector<int> duplicatedWordIds;
            for (const auto &[_, wordIndice] : mapReconstructedWordToWordIndice)
            {
                if (wordIndice.size() > 1)
                    duplicatedWordIds.insert(duplicatedWordIds.end(), wordIndice.begin(), wordIndice.end());
            }

            if (!duplicatedWordIds.empty())
            {
                int targetWordId = duplicatedWordIds[getRandomInt(0, duplicatedWordIds.size() - 1)];
                language.Words.erase(targetWordId); // mapのキー指定削除はO(log N)

                // ログ
                const auto dif = LanguageDifference::CreateObsoleteWord(place, Period, targetWordId);
                LanguageFamily_.languageDifference.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 言語に単語を追加する
 *
 * @param pCompound 単語追加律
 */
void LanguageFamilySimulator::MakeCompoundRandom(const double pCompound)
{
    for (auto &[place, language] : Languages)
    {
        // 単語を追加するかどうか
        if (getWithProbability(pCompound))
        {
            if (language.Words.empty())
            {
                return;
            }
            const auto randomWordID1 = getRandomInt(0, (int)language.Words.size() - 1);
            const auto randomWord1 = language.Words[randomWordID1];
            const auto randomWordID2 = getRandomInt(0, (int)language.Words.size() - 1);
            const auto randomWord2 = language.Words[randomWordID2];

            auto compound = randomWord1.Add(randomWord2);
            compound.UpdateReconstructedWord(ProtoLanguage);

            const int compoundID = language.Words.rbegin()->first + 1;
            language.Words[compoundID] = compound;

            // ログ出力
            const auto dif = LanguageDifference::CreateAddCompound(place, Period, compoundID, {randomWordID1, randomWordID2});
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
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]].GetForm() = converter.ConvertToPhoneme(diff.StringParam[1]);
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]].Meanings = diff.SemanticChange;
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        if (diff.DoubleParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }
        Languages[diff.StringParam[0]].Strength = diff.DoubleParam[0];
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        if (diff.IntParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }
        auto targetWordIterator = Languages[diff.StringParam[0]].Words.find(diff.IntParam[0]);
        if (targetWordIterator != Languages[diff.StringParam[0]].Words.end())
        {
            // 音韻変化を適用（インプレース更新）
            const auto &phonologicalChange = diff.PhonologicalChanges;
            auto converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable);
            if (converter.ConvertToString({phonologicalChange.BeforePhoneme}) == "" || converter.ConvertToString({phonologicalChange.AfterPhoneme}) == "")
            {
                return false;
            }
            std::vector<Phomene> changedWordForm;
            changedWordForm.reserve(targetWordIterator->second.GetForm().size());

            for (size_t phonemePosition = 0; phonemePosition < targetWordIterator->second.GetForm().size(); ++phonemePosition)
            {
                bool isEqualToBeforePhoneme = (targetWordIterator->second.GetForm()[phonemePosition] == phonologicalChange.BeforePhoneme);
                if (isEqualToBeforePhoneme)
                {
                    if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Start && phonemePosition != 0)
                        isEqualToBeforePhoneme = false;
                    else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::End && phonemePosition != targetWordIterator->second.GetForm().size() - 1)
                        isEqualToBeforePhoneme = false;
                    else if (phonologicalChange.PhoneticEnvironment == PhoneticEnvironment::Middle && (phonemePosition == 0 || phonemePosition == targetWordIterator->second.GetForm().size() - 1))
                        isEqualToBeforePhoneme = false;
                }
                if (isEqualToBeforePhoneme)
                {
                    if (!phonologicalChange.IsRemove)
                        changedWordForm.push_back(phonologicalChange.AfterPhoneme);
                }
                else
                {
                    changedWordForm.push_back(targetWordIterator->second.GetForm()[phonemePosition]);
                }
            }
            targetWordIterator->second.GetForm() = std::move(changedWordForm);
        }
        break;
    }

    case LanguageDifferenceType::SemanticChange:
    {
        if (diff.IntParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }
        auto targetWordIterator = Languages[diff.StringParam[0]].Words.find(diff.IntParam[0]);
        if (targetWordIterator != Languages[diff.StringParam[0]].Words.end())
        {
            targetWordIterator->second.Meanings = diff.SemanticChange;
            targetWordIterator->second.UpdateReconstructedWord(ProtoLanguage);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        if (diff.IntParam.size() < 2 || diff.StringParam.size() < 2)
        {
            return false;
        }
        if (Languages.count(diff.StringParam[0]) == 1)
        {
            const auto referenceLanguage = Languages.at(diff.StringParam[0]);
            if (referenceLanguage.Words.count(diff.IntParam[0]) == 1)
            {
                const auto referenceWord = referenceLanguage.Words.at(diff.IntParam[0]);
                Languages[diff.StringParam[1]].Words[diff.IntParam[1]] = referenceWord;
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
        WordForSimulation compound;
        // IntParam[1]以降に合成元の単語IDリストが格納されている
        for (size_t i = 1; i < diff.IntParam.size(); ++i)
        {
            auto itPart = Languages[diff.StringParam[0]].Words.find(diff.IntParam[i]);
            if (itPart != Languages[diff.StringParam[0]].Words.end())
            {
                compound = compound.Add(itPart->second);
            }
        }
        compound.UpdateReconstructedWord(ProtoLanguage);
        Languages[diff.StringParam[0]].Words[diff.IntParam[0]] = std::move(compound);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        if (diff.IntParam.size() < 1 || diff.StringParam.size() < 1)
        {
            return false;
        }
        Languages[diff.StringParam[0]].Words.erase(diff.IntParam[0]);
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
 * @brief 文字列の配列に変換
 *
 * @return std::vector<std::vector<std::string>> 配列
 */
std::vector<std::vector<std::string>> LanguageFamilySimulator::ToString()
{
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;

    // 1. 文字列変換の結果をキャッシュするマップ (高速化の肝)
    std::map<std::vector<Phomene>, std::string> convertCache;
    auto getCachedString = [&](const std::vector<Phomene> &s) -> const std::string &
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

    // 3. 祖語の単語との対応マップの作成
    // mapsReconstructedWordToWord[言語インデックス][祖語の音素列] -> 該当する単語リスト
    std::vector<std::map<std::vector<Phomene>, std::vector<const WordForSimulation *>>> mapsReconstructedWordToWord;
    mapsReconstructedWordToWord.resize(languagePtrList.size());
    for (size_t i = 0; i < languagePtrList.size(); ++i)
    {
        for (const auto &[_, word] : languagePtrList[i]->Words)
        {
            mapsReconstructedWordToWord[i][word.ReconstructedWord].push_back(&word); //
        }
    }

    // 4. 「Toki Pona」行の出力 (言語名の特定)

    // 5. 各単語の出力
    for (const auto &[_, reconstructedWord] : ProtoLanguage.Words)
    {
        const auto &reconstructedWordForm = reconstructedWord.GetForm();

        // この祖語単語に対して、各地点で最大何個の派生語があるか確認
        size_t maxDerivativeCount = 0;
        for (size_t languageIndex = 0; languageIndex < languagePtrList.size(); ++languageIndex)
        {
            maxDerivativeCount = std::max(maxDerivativeCount, mapsReconstructedWordToWord[languageIndex][reconstructedWordForm].size());
        }

        // 派生語の数だけ行を出力
        for (size_t row = 0; row < maxDerivativeCount; ++row)
        {
            if (row == 0)
                line.emplace_back(getCachedString(reconstructedWordForm));

            for (size_t languageIndex = 0; languageIndex < languagePtrList.size(); ++languageIndex)
            {
                const auto &derivedWords = mapsReconstructedWordToWord[languageIndex][reconstructedWordForm];
                if (row < derivedWords.size())
                {
                    line.emplace_back(getCachedString(derivedWords[row]->GetForm()));
                }
            }
            result.emplace_back(line);
            line.clear();
        }
    }
    return result;
}
#include "LanguageFamilySimulator.h"
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

    /**
     * @brief 祖語をセット
     *
     * @param geometryData 地理データ
     * @param startPlace 祖語を配置する地域
     * @param protoLanguage 祖語
     * @return std::map<std::string, Language>
     */
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
            language.Strength_ = 0.0;
            language.Words_ = {};

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
     * @brief 音韻の制限
     *
     * @param changedWordForm
     * @return true
     * @return false
     */
    bool CheckSoundDuplication(const std::vector<Phoneme> &changedWordForm)
    {
        // 子音と母音の境界
        constexpr int MAX_CONSONANT_MANNER = 3;

        bool isSoundDuplication = false;

        std::vector<std::vector<Phoneme>> wordForms;
        std::vector<Phoneme> wordForm;
        for (const auto &phoneme : changedWordForm)
        {
            if (phoneme.IsSpace_)
            {
                wordForms.emplace_back(wordForm);
                wordForm.clear();
            }
            else
            {
                wordForm.emplace_back(phoneme);
            }
        }
        wordForms.emplace_back(wordForm);

        for (const auto &w : wordForms)
        {
            if (w.empty())
                isSoundDuplication = true;
            else if (w.size() == 1)
            {
                if (w[0].Manner_ <= MAX_CONSONANT_MANNER)
                    isSoundDuplication = true;
            }
            else
            {
                // 境界条件のチェック
                if ((w[0].Manner_ <= MAX_CONSONANT_MANNER && w[1].Manner_ <= MAX_CONSONANT_MANNER) ||
                    (w.back().Manner_ <= MAX_CONSONANT_MANNER && w[w.size() - 2].Manner_ <= MAX_CONSONANT_MANNER))
                {
                    isSoundDuplication = true;
                }
                else
                {
                    // 3連続のチェック
                    for (size_t j = 0; j + 2 < w.size(); ++j)
                    {
                        bool isConsonant = (w[j].Manner_ <= MAX_CONSONANT_MANNER &&
                                            w[j + 1].Manner_ <= MAX_CONSONANT_MANNER &&
                                            w[j + 2].Manner_ <= MAX_CONSONANT_MANNER);
                        bool isVowel = (w[j].Manner_ > MAX_CONSONANT_MANNER &&
                                        w[j + 1].Manner_ > MAX_CONSONANT_MANNER &&
                                        w[j + 2].Manner_ > MAX_CONSONANT_MANNER);
                        if (isConsonant || isVowel)
                        {
                            isSoundDuplication = true;
                            break;
                        }
                    }
                }
            }
        }
        return isSoundDuplication;
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
        randomPhonologicalChange.BeforePhoneme_ = beforePhon;
        randomPhonologicalChange.IsRemove_ = getWithProbability(pRemoveSound);
        switch (randomPhoneticEnvironment)
        {
        case 0:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::Start;
            break;
        case 1:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::Middle;
            break;
        case 2:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::End;
            break;

        default:
            break;
        }
        randomPhonologicalChange.AfterPhoneme_ = beforePhon;
        moveRandomOnTable(randomPhonologicalChange.AfterPhoneme_.Manner_, randomPhonologicalChange.AfterPhoneme_.Place_, phonemeTable);
        return randomPhonologicalChange;
    }

    /**
     * @brief 音素表から、音素をランダムに1つ選択する
     * @param phonemeTable 音素表
     * @return 音素
     */
    Phoneme getRandomSoundFromTable(const std::vector<std::vector<std::string>> &phonemeTable)
    {
        const auto converter = PhonemeConverter::Create(phonemeTable);
        return converter.GetRandom();
    }

    /**
     * @brief 音韻変化を適用する
     *
     * @param wordForm 語形
     * @param changedWordForm 変化語の語形
     * @param phonologicalChange 音韻変化
     */
    bool ChangeWordSound(const std::vector<Phoneme> &wordForm, std::vector<Phoneme> &changedWordForm, const PhonologicalChange &phonologicalChange)
    {
        changedWordForm.reserve(wordForm.size());
        bool isChanged = false;

        for (size_t soundPosition = 0; soundPosition < wordForm.size(); ++soundPosition)
        {
            const auto &sound = wordForm[soundPosition];

            // 変化条件の判定
            bool isSoundEqualToBeforePhoneme = (sound == phonologicalChange.BeforePhoneme_);
            if (isSoundEqualToBeforePhoneme)
            {
                if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::Start && !(soundPosition == 0 || wordForm[soundPosition - 1].IsSpace_))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::End && !(soundPosition == wordForm.size() - 1 || wordForm[soundPosition + 1].IsSpace_))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::Middle && (soundPosition == 0 || wordForm[soundPosition - 1].IsSpace_ || soundPosition == wordForm.size() - 1 || wordForm[soundPosition + 1].IsSpace_))
                    isSoundEqualToBeforePhoneme = false;
            }

            if (isSoundEqualToBeforePhoneme)
            {
                isChanged = true;
                if (!phonologicalChange.IsRemove_)
                {
                    changedWordForm.push_back(phonologicalChange.AfterPhoneme_);
                }
            }
            else
            {
                changedWordForm.push_back(sound);
            }
        }

        return isChanged;
    }
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
    Languages_ = setProtoLanguageOnMap(getNonEmptyStrings(LanguageFamily_.Geography_), startPlace, protoLanguage);
    ProtoLanguage_ = protoLanguage;

    // ログ
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);
    LanguageFamily_.languageDifference_.emplace_back(LanguageDifference::CreateChangeStrength(startPlace, Period_, protoLanguage.Strength_));
    for (const auto &[wordID, word] : protoLanguage.Words_)
    {
        LanguageFamily_.languageDifference_.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period_, wordID, converter.ConvertToString(word.Form_)));
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
    if (Languages_.count(place) == 0)
    {
        return {};
    }
    const auto language = Languages_[place];
    std::vector<std::string> words;
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);
    for (const auto &[_, word] : language.Words_)
    {
        words.emplace_back(converter.ConvertToString(word.Form_));
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
    // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
    std::map<int, Word> phonologicalChangedWords;

    // 1. 音韻変化の適用と音素重複チェックを同時に行う
    for (auto &[wordID, word] : language.Words_)
    {
        std::vector<Phoneme> changedWordForm;
        changedWordForm.reserve(word.Form_.size());

        if (!ChangeWordSound(word.Form_, changedWordForm, phonologicalChange))
            continue;

        // 子音・母音の重複禁止チェック (isSoundDuplication)
        if (isProhibitSoundDuplication)
        {
            if (CheckSoundDuplication(changedWordForm))
                continue; // 違反していればこの単語の変化は破棄
        }

        // 変化後の単語候補を一時保存
        Word changedWord = word;
        changedWord.Form_ = std::move(changedWordForm); // 所有権を移転してコピーを回避
        phonologicalChangedWords[wordID] = std::move(changedWord);
    }

    // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
    if (isProhibitMinimalPair)
    {
        // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
        std::map<std::vector<Phoneme>, int> mimimalPairCount;
        for (const auto &[wordID, word] : language.Words_)
        {
            auto it = phonologicalChangedWords.find(wordID);
            mimimalPairCount[it != phonologicalChangedWords.end() ? it->second.Form_ : word.Form_]++;
        }

        // 重複が発生する変化を差し止める
        for (auto it = phonologicalChangedWords.begin(); it != phonologicalChangedWords.end();)
        {
            if (mimimalPairCount[it->second.Form_] > 1)
                it = phonologicalChangedWords.erase(it);
            else
                ++it;
        }
    }

    // 3. 最終的な反映（一括代入）
    for (auto &[wordID, phonologicalChangedWord] : phonologicalChangedWords)
    {
        language.Words_[wordID] = std::move(phonologicalChangedWord);
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
    for (auto &[place, language] : Languages_)
    {
        // 音韻変化するかどうか
        if (!getWithProbability(pPhonologicalChange))
        {
            continue;
        }
        // 言語があるか
        if (language.Words_.empty())
        {
            continue;
        }
        const auto randomSound = getRandomSoundFromTable(LanguageFamily_.PhonemeTable_);
        PhonologicalChange randomPhonologicalChange = makepPhonologicalChangeRandom(randomSound, LanguageFamily_.PhonemeTable_, pSoundLoss);

        // ログ
        const auto dif = LanguageDifference::CreatePhonologicalChange(place, Period_, randomPhonologicalChange);
        LanguageFamily_.languageDifference_.emplace_back(dif);

        ApplyPhonologicalChange(language, randomPhonologicalChange, isProhibitMinimalPair, isProhibitSoundDuplication);
    }
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
    const auto geographyAdjacentData = getAdjacencies(LanguageFamily_.Geography_);
    for (int i = 0; i < nLoanword; i++)
    {
        // 借用率 は現在固定
        const auto adjucent = geographyAdjacentData[getRandomInt(0, geographyAdjacentData.size() - 1)];
        {
            auto languageIterator1 = Languages_.find(adjucent.first);
            auto languageIterator2 = Languages_.find(adjucent.second);
            if (languageIterator1 == Languages_.end() || languageIterator2 == Languages_.end())
                continue;

            Language &language1 = languageIterator1->second;
            Language &language2 = languageIterator2->second;

            if (language1.Words_.empty() || language2.Words_.empty())
            {
                if (language1.Words_.empty())
                {
                    language1.Words_ = language2.Words_;
                    language1.Strength_ = language2.Strength_;

                    // ログ
                    for (const auto &[wordID, word] : language2.Words_)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(languageIterator2->first, languageIterator1->first, Period_, wordID, wordID);
                        LanguageFamily_.languageDifference_.emplace_back(dif);
                    }
                }
                else
                {
                    language2.Words_ = language1.Words_;
                    language2.Strength_ = language1.Strength_;

                    // ログ
                    for (const auto &[wordID, word] : language1.Words_)
                    {
                        const auto dif = LanguageDifference::CreateLoanword(languageIterator1->first, languageIterator2->first, Period_, wordID, wordID);
                        LanguageFamily_.languageDifference_.emplace_back(dif);
                    }
                }
                continue;
            }

            auto *referenceLanguage = (language1.Strength_ > language2.Strength_) ? &language1 : &language2;
            auto *targetLanguage = (language1.Strength_ > language2.Strength_) ? &language2 : &language1;
            const auto referencePlace = (language1.Strength_ > language2.Strength_) ? languageIterator1->first : languageIterator2->first;
            const auto targetPlace = (language1.Strength_ > language2.Strength_) ? languageIterator2->first : languageIterator1->first;

            for (auto &[targetWordID, targetWord] : targetLanguage->Words_)
            {
                // 借用率は 0.5 で固定
                if (getRandomInt(0, 1) != 0)
                    continue;

                targetWord = referenceLanguage->Words_[targetWordID];

                // ログ
                const auto dif = LanguageDifference::CreateLoanword(referencePlace, targetPlace, Period_, targetWordID, targetWordID);
                LanguageFamily_.languageDifference_.emplace_back(dif);
            }
        }
    }
}

/**
 * @brief 言語の影響度をランダムに変化させる
 *
 * @param pChangeStrength 変化率
 */
void LanguageFamilySimulator::ChangeLanguageStrengthRandom(const double pChangeStrength)
{
    for (auto &[place, language] : Languages_)
    {
        if (getWithProbability(pChangeStrength))
        {
            language.Strength_ = language.Strength_ * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;

            // ログ
            const auto dif = LanguageDifference::CreateChangeStrength(place, Period_, language.Strength_);
            LanguageFamily_.languageDifference_.emplace_back(dif);
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
    const auto places = getNonEmptyStrings(LanguageFamily_.Geography_);
    for (const auto &place : places)
    {
        // find を使うことで「存在チェック」と「データアクセス」を1回で済ませる
        auto it = Languages_.find(place);
        if (it == Languages_.end() || it->second.Words_.empty())
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
    Period_++;
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
    const auto places = getNonEmptyStrings(LanguageFamily_.Geography_);
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);

    switch (diff.Type_)
    {
    case LanguageDifferenceType::AddWord:
    {
        if (diff.IntParam_.size() < 1 || diff.StringParam_.size() < 2)
        {
            return false;
        }

        const auto geometry = diff.StringParam_[0];
        const auto wordID = diff.IntParam_[0];
        const auto form = diff.StringParam_[1];

        Languages_[geometry].Words_[wordID].Form_ = converter.ConvertToPhoneme(form);
        if (diff.Period_ == 0 && geometry == "0")
        {
            ProtoLanguage_.Words_[wordID].Form_ = converter.ConvertToPhoneme(form);
        }
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        if (diff.DoubleParam_.size() < 1 || diff.StringParam_.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam_[0];
        const auto strength = diff.DoubleParam_[0];

        Languages_[geometry].Strength_ = strength;
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        if (diff.StringParam_.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam_[0];

        if (Languages_.count(geometry) == 1)
        {
            ApplyPhonologicalChange(Languages_[geometry], diff.PhonologicalChanges_, true, true);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        if (diff.IntParam_.size() < 2 || diff.StringParam_.size() < 2)
        {
            return false;
        }

        const auto referenceGeometry = diff.StringParam_[0];
        const auto targetGeometry = diff.StringParam_[1];
        const auto referenceWordID = diff.IntParam_[0];
        const auto targetWordID = diff.IntParam_[1];

        if (Languages_.count(referenceGeometry) == 1)
        {
            const auto referenceLanguage = Languages_.at(referenceGeometry);
            if (referenceLanguage.Words_.count(referenceWordID) == 1)
            {
                const auto referenceWord = referenceLanguage.Words_.at(referenceWordID);
                Languages_[targetGeometry].Words_[targetWordID] = referenceWord;
            }
        }
        break;
    }

    case LanguageDifferenceType::AddCompound:
    {
        if (diff.IntParam_.size() < 2 || diff.StringParam_.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam_[0];
        const auto wordID = diff.IntParam_[0];

        Word compound;
        // IntParam[1]以降に合成元の単語IDリストが格納されている
        for (size_t i = 1; i < diff.IntParam_.size(); ++i)
        {
            auto itPart = Languages_[geometry].Words_.find(diff.IntParam_[i]);
            if (itPart != Languages_[geometry].Words_.end())
            {
                compound = compound.Add(itPart->second);
            }
        }
        Languages_[geometry].Words_[wordID] = std::move(compound);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        if (diff.IntParam_.size() < 1 || diff.StringParam_.size() < 1)
        {
            return false;
        }

        const auto geometry = diff.StringParam_[0];
        const auto wordID = diff.IntParam_[0];

        Languages_[geometry].Words_.erase(wordID);
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

std::optional<Language> LanguageFamilySimulator::CalculateLanguage(const std::string place, const int period)
{
    for (const auto &diff : LanguageFamily_.languageDifference_)
    {
        if (diff.Period_ > period)
        {
            return Languages_.at(place);
        }
        if (!ApplyDifference(diff))
        {
            return std::nullopt;
        }
    }
    return Languages_.at(place);
}

/**
 * @brief インスタンス生成
 *
 */
std::optional<LanguageFamilySimulator> LanguageFamilySimulator::Create()
{
    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_.languageDifference_ = {};
    simulator.LanguageFamily_.Geography_ = {{""}};
    simulator.LanguageFamily_.PhonemeTable_ = {{""}};
    simulator.Period_ = 0;
    simulator.Languages_.clear();
    simulator.ProtoLanguage_ = Language();

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
    simulator.Period_ = 0;
    simulator.Languages_.clear();
    simulator.ProtoLanguage_ = Language();

    if (!simulator.ApplyDifferences(languageFamily.languageDifference_))
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
    Period_ = 0;
    Languages_.clear();

    auto converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography_))
    {
        line.emplace_back(place);
    }
    result.emplace_back(line);
    line.clear();

    for (const auto &diff : LanguageFamily_.languageDifference_)
    {
        if (diff.Period_ != currentPeriod)
        {
            currentPeriod = diff.Period_;
            for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography_))
            {
                if (Languages_.count(place) == 0 || Languages_[place].Words_.empty())
                {
                    line.emplace_back("");
                }
                else
                {
                    line.emplace_back(converter.ConvertToString(Languages_[place].Words_[0].Form_));
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

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.Geography_))
    {
        if (Languages_.count(place) == 0 || Languages_[place].Words_.empty())
        {
            line.emplace_back("");
        }
        else
        {
            line.emplace_back(converter.ConvertToString(Languages_[place].Words_[0].Form_));
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
        PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);
        return convertCache[s] = converter.ConvertToString(s); //
    };

    // 2. ヘッダー行 (Place) の出力と、Languageポインタのキャッシュ
    line.emplace_back("");
    std::vector<const Language *> languagePtrList;
    languagePtrList.reserve(Languages_.size());
    for (const auto &[place, language] : Languages_)
    {
        line.emplace_back(place);
        languagePtrList.push_back(&language); // ループ内でのmap検索を避けるために保持
    }
    result.emplace_back(line);
    line.clear();

    // 5. 各単語の出力
    for (const auto &[wordID, _] : ProtoLanguage_.Words_)
    {
        line.emplace_back(getCachedString(ProtoLanguage_.Words_.at(wordID).Form_));
        for (const auto &[__, language] : Languages_)
        {
            line.emplace_back(getCachedString(language.Words_.at(wordID).Form_));
        }
        result.emplace_back(line);
        line.clear();
    }
    return result;
}
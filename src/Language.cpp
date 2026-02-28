#include "Language.h"
#include "Random.h"

namespace
{
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
 * @brief リセット
 *
 */
void Language::Reset()
{
    Strength_ = 0.0;
    Words_ = {};
}

/**
 * @brief コピー
 *
 * @param lang
 */
void Language::Copy(const Language &lang)
{
    Strength_ = lang.Strength_;
    Words_ = lang.Words_;
}

/**
 * @brief 音韻変化を言語に適用
 *
 * @param language 言語
 * @param phonologicalChange 音韻変化
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 * @param isSoundDuplication 音素の重複を禁止するか
 */
void Language::ApplyPhonologicalChange(const PhonologicalChange &phonologicalChange, const bool isProhibitSoundDuplication, const bool isProhibitMinimalPair)
{
    // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
    std::map<int, Word> phonologicalChangedWords;

    // 1. 音韻変化の適用と音素重複チェックを同時に行う
    for (auto &[wordID, word] : Words_)
    {
        auto changedWord = word;
        changedWord.ChangeSound(phonologicalChange, isProhibitSoundDuplication);

        if (word != changedWord)
        {
            phonologicalChangedWords[wordID] = std::move(changedWord);
        }
    }

    // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibiteMinimalPair)
    if (isProhibitMinimalPair)
    {
        // 現在の言語全体の単語分布を把握（変化しなかった単語 + 変化候補）
        std::map<std::vector<Phoneme>, int> mimimalPairCount;
        for (const auto &[wordID, word] : Words_)
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
        Words_[wordID] = std::move(phonologicalChangedWord);
    }
}

/**
 * @brief 差分を適用
 *
 * @param dif
 */
void Language::ApplyDifference(const LanguageDifference &dif)
{
    switch (dif.GetType())
    {
    case LanguageDifferenceType::ChangeStrength:
    {
        const auto strength = dif.DoubleParam(0);
        if (!strength)
        {
            break;
        }
        Strength_ = *strength;
        break;
    }
    case LanguageDifferenceType::PhonologicalChange:
        ApplyPhonologicalChange(dif.GetPhonologicalChange(), true, true);
        break;
    case LanguageDifferenceType::AddCompound:
    {
        Word compound;
        // IntParam[1]以降に合成元の単語IDリストが格納されている
        for (size_t i = 1; i < dif.IntParamSize(); ++i)
        {
            const auto referenceWordID = dif.IntParam(i);
            auto itPart = Words_.find(*referenceWordID);
            if (itPart != Words_.end())
            {
                compound = compound.Add(itPart->second);
            }
        }
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        Words_[*wordID] = std::move(compound);
        break;
    }
    case LanguageDifferenceType::ObsoleteWord:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        Words_.erase(*wordID);
        break;
    }

    default:
        break;
    }
}

/**
 * @brief 単語を追加
 *
 * @param dif 差分
 * @param form 語形
 */
void Language::AddWord(const LanguageDifference &dif, const std::vector<Phoneme> &form)
{
    const auto wordID = dif.IntParam(0);
    if (!wordID)
    {
        return;
    }
    Words_[*wordID] = Word::Create(form);
}

/**
 * @brief 単語を追加
 *
 * @param form 語形
 */
void Language::AddWord(const std::vector<Phoneme> &form)
{
    int lastWordID;
    if (Empty())
    {
        lastWordID = 0;
    }
    else
    {
        lastWordID = Words_.rbegin()->first;
        lastWordID++;
    }

    Words_[lastWordID] = Word::Create(form);
}

/**
 * @brief 単語を借用
 *
 * @param dif
 * @param referenceLanguage
 */
void Language::LoanWord(const LanguageDifference &dif, const Language &referenceLanguage)
{
    switch (dif.GetType())
    {
    case LanguageDifferenceType::Loanword:
    {
        const auto referenceWordID = dif.IntParam(0);
        if (!referenceWordID)
        {
            break;
        }
        const auto referenceWord = referenceLanguage.GetWord(*referenceWordID);
        if (referenceWord)
        {
            const auto wordID = dif.IntParam(1);
            if (!wordID)
            {
                break;
            }
            Words_[*wordID] = *referenceWord;
        }
        break;
    }

    default:
        break;
    }
}

/**
 * @brief Get the Strength object
 *
 * @return double
 */
const double Language::GetStrength() const
{
    return Strength_;
}

/**
 * @brief 影響度が強いか
 *
 * @param lang
 * @return true
 * @return false
 */
const bool Language::IsStronger(const Language &lang) const
{
    return Strength_ > lang.Strength_;
}

/**
 * @brief 影響度を変更する。
 *
 * @param place 地域
 * @param period 時代
 * @return LanguageDifference
 */
const LanguageDifference Language::ChangeStrength(const std::string &place, const int period)
{
    Strength_ = Strength_ * 0.9 + getRandomDouble(-1.0, 1.0) * 0.1;
    return LanguageDifference::CreateChangeStrength(place, period, Strength_);
}

/**
 * @brief 単語数
 *
 * @return int
 */
const int Language::CountWord() const
{
    return Words_.size();
}

/**
 * @brief Get the Nth Word object
 *
 * @return std::pair<int, Word>
 */
const std::pair<int, Word> Language::GetNthWord(const int n) const
{
    auto it = std::next(Words_.begin(), n);
    return {it->first, it->second};
}

/**
 * @brief 単語を取得
 *
 * @param wordID
 * @return const Word
 */
const std::optional<Word> Language::GetWord(const int wordID) const
{
    auto it = Words_.find(wordID);
    if (it == Words_.end())
        return std::nullopt;
    return it->second;
}

/**
 * @brief 単語が空か
 *
 * @return true
 * @return false
 */
const bool Language::Empty() const
{
    return Words_.empty();
}
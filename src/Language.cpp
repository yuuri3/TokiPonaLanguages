#include "Language.h"
#include "Random.h"
#include "PhonemeConverter.h"
#include "LanguageDifference.h"

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
const std::pair<const int, Word> &Language::GetNthWord(const int n) const
{
    auto it = std::next(Words_.begin(), n);
    return *it;
}

/**
 * @brief 単語を取得
 *
 * @param wordID
 * @return const Word
 */
const Word *Language::GetWord(const int wordID) const
{
    auto it = Words_.find(wordID);
    if (it == Words_.end())
        return nullptr;
    return &(it->second);
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

/**
 * @brief 差分を適用
 *
 * @param diff 差分
 *
 * @return 成否
 */
bool LanguageUtility::ApplyDifference(const LanguageDifference &diff, std::map<std::string, Language> &languages, const PhonemeConverter &converter)
{
    // const auto places = getNonEmptyStrings(LanguageFamily_.GetGeography());

    switch (diff.GetType())
    {
    case LanguageDifferenceType::AddWord:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        const auto wordID = diff.IntParam(0);
        if (!wordID)
        {
            return false;
        }
        const auto form = diff.StringParam(1);
        if (!form)
        {
            return false;
        }

        languages[*geometry].AddWord(diff, converter.ConvertToPhoneme(*form));
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }

        if (languages.count(*geometry) == 1)
        {
            languages[*geometry].ApplyDifference(diff);
        }
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }

        if (languages.count(*geometry) == 1)
        {
            languages[*geometry].ApplyDifference(diff);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        const auto referenceGeometry = diff.StringParam(0);
        if (!referenceGeometry)
        {
            return false;
        }
        const auto targetGeometry = diff.StringParam(1);
        if (!targetGeometry)
        {
            return false;
        }
        const auto referenceWordID = diff.IntParam(0);
        if (!referenceWordID)
        {
            return false;
        }
        const auto targetWordID = diff.IntParam(1);
        if (!targetWordID)
        {
            return false;
        }

        if (languages.count(*referenceGeometry) == 1)
        {
            const auto referenceLanguage = languages.at(*referenceGeometry);
            languages[*targetGeometry].LoanWord(diff, referenceLanguage);
        }
        break;
    }

    case LanguageDifferenceType::AddCompound:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        languages[*geometry].ApplyDifference(diff);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        languages[*geometry].ApplyDifference(diff);
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
bool LanguageUtility::ApplyDifferences(const std::vector<LanguageDifference> &diffs, std::map<std::string, Language> &languages, const PhonemeConverter &converter)
{
    for (const auto &diff : diffs)
    {
        if (!ApplyDifference(diff, languages, converter))
        {
            return false;
        }
    }
    return true;
}
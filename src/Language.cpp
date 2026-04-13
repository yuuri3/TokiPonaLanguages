#include "Language.h"
#include "Random.h"
#include "LanguageDifference.h"
#include "LanguageFamily.h"

Language::Language(const LanguageFamily *parent)
{
    Parent_ = parent;
}

Language::~Language()
{
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
 * @param phonologicalChange 音韻変化
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 */
void Language::ApplyPhonologicalChange(const PhonologicalChange &phonologicalChange, const bool isProhibitMinimalPair)
{
    if (!Parent_)
    {
        return;
    }

    // 変更が発生した単語を記録する一時的なマップ（インプレース更新用）
    std::unordered_map<int, Word> phonologicalChangedWords;

    // 1. 音韻変化の適用
    for (auto &[wordId, word] : Words_)
    {
        auto changedWord = word;
        // Word内部で新しいPhonologicalChange（IDベース）に基づき変換を行う
        changedWord.ChangeSound(phonologicalChange, Parent_->GetPhonemeTable());

        // 音素IDの配列（std::vector<int>）が変化したかチェック
        if (word.GetForm() != changedWord.GetForm())
        {
            phonologicalChangedWords[wordId] = std::move(changedWord);
        }
    }

    // 2. 同音語（ミニマル・ペア）の禁止チェック (isProhibitMinimalPair)
    if (isProhibitMinimalPair)
    {
        // 現在の言語全体の単語分布を把握（音素IDの配列をキーにしてカウント）
        std::map<std::vector<int>, int> formCount;
        for (const auto &[wordId, word] : Words_)
        {
            auto it = phonologicalChangedWords.find(wordId);
            // 変化が予定されている単語は変化後のフォームを、そうでないものは現在のフォームを使用
            const auto &finalForm = (it != phonologicalChangedWords.end()) ? it->second.GetForm() : word.GetForm();
            formCount[finalForm]++;
        }

        // 重複が発生する変化（適用すると他の語と同音になってしまうケース）を差し止める
        for (auto it = phonologicalChangedWords.begin(); it != phonologicalChangedWords.end();)
        {
            if (formCount[it->second.GetForm()] > 1)
            {
                // 他の単語と衝突するため、この変化は採用しない
                it = phonologicalChangedWords.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // 3. 最終的な反映（一括代入）
    for (auto &[wordId, changedWord] : phonologicalChangedWords)
    {
        Words_[wordId] = std::move(changedWord);
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
    case LanguageDifferenceType::AddWord:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto form = dif.GetPhonemeParam();
        Words_[*wordID] = Word::Create(form);
        break;
    }
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
        ApplyPhonologicalChange(dif.GetPhonologicalChange(), true);
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
    case LanguageDifferenceType::EditPart:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto partID = dif.IntParam(1);
        if (!partID)
        {
            break;
        }
        const auto part = dif.StringParam(0);
        if (!part)
        {
            break;
        }
        Words_[*wordID].SetPart(*partID, *part);
        break;
    }
    case LanguageDifferenceType::EditTranslation:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto partID = dif.IntParam(1);
        if (!partID)
        {
            break;
        }
        const auto translationID = dif.IntParam(2);
        if (!translationID)
        {
            break;
        }
        const auto translation = dif.StringParam(0);
        if (!translation)
        {
            break;
        }
        Words_[*wordID].SetTranslation(*partID, *translationID, *translation);
        break;
    }
    case LanguageDifferenceType::DeletePart:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto partID = dif.IntParam(1);
        if (!partID)
        {
            break;
        }
        Words_[*wordID].DeletePart(*partID);
        break;
    }
    case LanguageDifferenceType::DeleteTranslation:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto partID = dif.IntParam(1);
        if (!partID)
        {
            break;
        }
        const auto translationID = dif.IntParam(2);
        if (!translationID)
        {
            break;
        }
        Words_[*wordID].DeleteTranslation(*partID, *translationID);
        break;
    }
    case LanguageDifferenceType::EditTag:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto tagID = dif.IntParam(1);
        if (!tagID)
        {
            break;
        }
        const auto tag = dif.StringParam(0);
        if (!tag)
        {
            break;
        }
        Words_[*wordID].SetTag(*tagID, *tag);
        break;
    }
    case LanguageDifferenceType::DeleteTag:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto tagID = dif.IntParam(1);
        if (!tagID)
        {
            break;
        }
        Words_[*wordID].DeleteTag(*tagID);
        break;
    }
    case LanguageDifferenceType::EditContent:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto contentID = dif.IntParam(1);
        if (!contentID)
        {
            break;
        }
        const auto title = dif.StringParam(0);
        if (!title)
        {
            break;
        }
        const auto content = dif.StringParam(1);
        if (!content)
        {
            break;
        }
        Words_[*wordID].SetContent(*contentID, *title, *content);
        break;
    }
    case LanguageDifferenceType::DeleteContent:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto contentID = dif.IntParam(1);
        if (!contentID)
        {
            break;
        }
        Words_[*wordID].DeleteContent(*contentID);
        break;
    }
    case LanguageDifferenceType::EditVariation:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto variationID = dif.IntParam(1);
        if (!variationID)
        {
            break;
        }
        const auto title = dif.StringParam(0);
        if (!title)
        {
            break;
        }
        const auto variationForm = dif.GetPhonemeParam();
        Words_[*wordID].SetVariation(*variationID, *title, variationForm);
        break;
    }
    case LanguageDifferenceType::DeleteVariation:
    {
        const auto wordID = dif.IntParam(0);
        if (!wordID)
        {
            break;
        }
        const auto variationID = dif.IntParam(1);
        if (!variationID)
        {
            break;
        }
        Words_[*wordID].DeleteVariation(*variationID);
        break;
    }
    case LanguageDifferenceType::SetRelation:
    {
        const auto wordID = dif.IntParam(0);
        const auto relationID = dif.IntParam(1);
        const auto targetWordID = dif.IntParam(2);
        const auto title = dif.StringParam(0); // 0はplace

        if (!wordID || !relationID || !targetWordID || !title)
        {
            break;
        }

        Words_[*wordID].SetRelation(*relationID, *title, *targetWordID);
        break;
    }
    case LanguageDifferenceType::DeleteRelation:
    {
        const auto wordID = dif.IntParam(0);
        const auto relationID = dif.IntParam(1);

        if (!wordID || !relationID)
        {
            break;
        }

        Words_[*wordID].DeleteRelation(*relationID);
        break;
    }

    default:
        break;
    }
}

/**
 * @brief 単語を追加
 *
 * @param form 語形
 */
void Language::AddWord(const std::vector<int> &form)
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
const LanguageDifference Language::ChangeStrength(const int &place, const int period)
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
 * @brief 新しい単語IDを取得
 *
 * @return const int
 */
const int Language::GetNewWordID() const
{
    if (Empty())
    {
        return 0;
    }
    return Words_.rbegin()->first + 1;
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
bool LanguageUtility::ApplyDifference(const LanguageDifference &diff, std::map<int, Language> &languages, const LanguageFamily *family)
{
    // const auto places = getNonEmptyStrings(LanguageFamily_.GetGeography());

    switch (diff.GetType())
    {
    case LanguageDifferenceType::Loanword:
    {
        const auto referenceGeometry = diff.IntParam(0);
        if (!referenceGeometry)
        {
            return false;
        }
        const auto targetGeometry = diff.GetPlace();

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
            languages.at(targetGeometry).LoanWord(diff, referenceLanguage);
        }
        break;
    }
    default:
    {
        const auto geometry = diff.GetPlace();

        if (languages.count(geometry) == 0)
        {
            languages.emplace(geometry, family);
        }
        languages.at(geometry).ApplyDifference(diff);
        break;
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
bool LanguageUtility::ApplyDifferences(const std::vector<LanguageDifference> &diffs, std::map<int, Language> &languages, const LanguageFamily *family)
{
    for (const auto &diff : diffs)
    {
        if (!ApplyDifference(diff, languages, family))
        {
            return false;
        }
    }
    return true;
}
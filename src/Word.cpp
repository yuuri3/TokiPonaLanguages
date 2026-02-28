#include "..\\include\Word.h"
#include "..\\include\Language.h"

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
 * @brief 複合語を生成
 *
 * @param word 単語
 * @return 複合語
 */
Word Word::Add(const Word &word) const
{
    Word result;
    result.Form_.reserve(Form_.size() + word.Form_.size());
    result.Form_.insert(result.Form_.end(), Form_.begin(), Form_.end());
    result.Form_.insert(result.Form_.end(), word.Form_.begin(), word.Form_.end());

    return result;
}

/**
 * @brief インスタンス作成
 *
 * @param form
 */
Word Word::Create(const std::vector<Phoneme> &form)
{
    Word word;
    word.Form_ = form;
    return word;
}

/**
 * @brief 語形をゲット
 *
 * @return const std::vector<Phoneme>
 */
const std::vector<Phoneme> Word::GetForm() const
{
    return Form_;
}

/**
 * @brief 訳語をゲット
 *
 * @return const std::map<std::string, std::vector<std::string>>
 */
const std::map<std::string, std::vector<std::string>> Word::GetTranslations() const
{
    return Translations_;
}

/**
 * @brief タグをゲット
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Word::GetTags() const
{
    return Tags_;
}

/**
 * @brief 自由記述をゲット
 *
 * @return const std::map<std::string, std::string>
 */
const std::map<std::string, std::string> Word::GetContents() const
{
    return Contents_;
}

/**
 * @brief 変化形をゲット
 *
 * @return const std::map<std::string, std::vector<Phoneme>>
 */
const std::map<std::string, std::vector<Phoneme>> Word::GetVariations() const
{
    return Variations_;
}

/**
 * @brief 関連語をゲット
 *
 * @return const std::map<std::string, int>
 */
const std::map<std::string, int> Word::GetRealtions() const
{
    return Relations_;
}

/**
 * @brief 訳語の集合を取得
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Word::GetAllTranslations() const
{
    std::vector<std::string> result;
    for (const auto &[_, translations] : Translations_)
    {
        result.insert(result.end(), translations.begin(), translations.end());
    }
    return result;
}

/**
 * @brief 音韻変化
 *
 * @param phon
 */
void Word::ChangeSound(PhonologicalChange phon, const bool isProhibitSoundDuplication)
{
    std::vector<Phoneme> changedWordForm;
    if (!ChangeWordSound(Form_, changedWordForm, phon))
        return;

    // 子音・母音の重複禁止チェック (isSoundDuplication)
    if (isProhibitSoundDuplication)
    {
        if (CheckSoundDuplication(changedWordForm))
            return; // 違反していればこの単語の変化は破棄
    }
    Form_ = changedWordForm;
}
#include "Word.h"
#include "Language.h"

/**
 * @brief 複合語を生成
 *
 * @param word 単語
 * @return 複合語
 */
Word Word::Add(const Word &word) const
{
    Word result;
    result.Form.reserve(Form.size() + word.Form.size());
    result.Form.insert(result.Form.end(), Form.begin(), Form.end());
    result.Form.insert(result.Form.end(), word.Form.begin(), word.Form.end());

    result.Meanings = Meanings.Add(word.Meanings);
    result.Meanings.Normalize();
    return result;
}

/**
 * @brief reconstructedWordを更新する
 *
 * @param protoLanguage 祖語
 */
void Word::UpdateReconstructedWord(const Language &protoLanguage)
{
    double maxDot = -1.0;
    for (const auto &[_, word] : protoLanguage.Words)
    {
        const double dot = Meanings.Dot(word.Meanings);
        if (dot > maxDot)
        {
            maxDot = dot;
            ReconstructedWord = word.Form;
        }
    }
}
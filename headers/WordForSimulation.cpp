#include "WordForSimulation.h"
#include "Language.h"

/**
 * @brief 複合語を生成
 *
 * @param word 単語
 * @return 複合語
 */
WordForSimulation WordForSimulation::Add(const WordForSimulation &word) const
{
    WordForSimulation result;
    result.Add(word);

    result.Meanings = Meanings.Add(word.Meanings);
    result.Meanings.Normalize();
    return result;
}

/**
 * @brief reconstructedWordを更新する
 *
 * @param protoLanguage 祖語
 */
void WordForSimulation::UpdateReconstructedWord(const Language &protoLanguage)
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
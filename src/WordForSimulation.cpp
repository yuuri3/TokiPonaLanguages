#include "..\\include\WordForSimulation.h"
#include "..\\include\Language.h"

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
    double maxMeaningDot = -1.0;
    for (const auto &[_, reconstructedWord] : protoLanguage.Words)
    {
        const double meaningDot = Meanings.Dot(reconstructedWord.Meanings);
        if (meaningDot > maxMeaningDot)
        {
            maxMeaningDot = meaningDot;
            ReconstructedWord = reconstructedWord.Word_.Form;
        }
    }
}

/**
 * @brief Form を返す
 *
 * @return std::vector<Phomene>& Form
 */
std::vector<Phomene> &WordForSimulation::GetForm()
{
    return Word_.Form;
}

/**
 * @brief Form を返す
 *
 * @return std::vector<Phomene>& Form
 */
std::vector<Phomene> WordForSimulation::GetForm() const
{
    return Word_.Form;
}
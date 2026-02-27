#include "..\\include\Word.h"
#include "..\\include\Language.h"

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
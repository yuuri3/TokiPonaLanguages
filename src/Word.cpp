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
    result.Form.reserve(Form.size() + word.Form.size());
    result.Form.insert(result.Form.end(), Form.begin(), Form.end());
    result.Form.insert(result.Form.end(), word.Form.begin(), word.Form.end());

    return result;
}
/**
 * @brief 語族差分タイプ
 *
 */
enum class LanguageDifferenceType
{
    // 単語追加
    // string 地理
    // int 単語ID
    // string 語形
    // Meaning 意味
    AddWord = 0,
    // 影響度変化
    // string 地理
    // double 影響度
    ChangeStrength = 1,
    // 音韻変化
    // string 地理
    // PhonologicalChange 音韻変化
    PhonologicalChange = 2,
    // 借用
    // string 地理
    // int 借用元単語ID
    // string 地理
    // int 借用先単語ID
    Loanword = 4,
    // 複合語
    // string 地理
    // int 単語ID
    // int... 参照単語ID
    AddCompound = 5,
    // 死語
    // string 地理
    // int 単語ID
    ObsoleteWord = 6
};
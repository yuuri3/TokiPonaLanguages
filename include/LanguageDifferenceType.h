/**
 * @brief 語族差分タイプ
 *
 */
enum class LanguageDifferenceType
{
    UnDefined = -1,
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

inline const LanguageDifferenceType ConvertToLanguageDifferenceType(const int i)
{
    switch (i)
    {
    case 0:
        return LanguageDifferenceType::AddWord;
    case 1:
        return LanguageDifferenceType::ChangeStrength;
    case 2:
        return LanguageDifferenceType::PhonologicalChange;
    case 4:
        return LanguageDifferenceType::Loanword;
    case 5:
        return LanguageDifferenceType::AddCompound;
    case 6:
        return LanguageDifferenceType::ObsoleteWord;
    default:
        return LanguageDifferenceType::UnDefined;
    }
}
inline const int ConvertFromLanguageDifferenceType(const LanguageDifferenceType type)
{
    switch (type)
    {
    case LanguageDifferenceType::AddWord:
        return 0;
    case LanguageDifferenceType::ChangeStrength:
        return 1;
    case LanguageDifferenceType::PhonologicalChange:
        return 2;
    case LanguageDifferenceType::Loanword:
        return 4;
    case LanguageDifferenceType::AddCompound:
        return 5;
    case LanguageDifferenceType::ObsoleteWord:
        return 6;
    default:
        return -1;
    }
}
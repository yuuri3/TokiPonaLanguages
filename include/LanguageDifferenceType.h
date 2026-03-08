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
    ObsoleteWord = 6,
    // 品詞追加
    // string 地理
    // int 単語ID
    // string 品詞ID
    // string 変更後の品詞
    EditPart = 7,
    // 品詞削除
    // string 地理
    // int 単語ID
    // string 品詞ID
    DeletePart = 8,
    // 品詞追加
    // string 地理
    // int 単語ID
    // int 品詞ID
    // int 訳語ID
    // string 変更後の訳語
    EditTranslation = 9,
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
    case 7:
        return LanguageDifferenceType::EditPart;
    case 8:
        return LanguageDifferenceType::DeletePart;
    case 9:
        return LanguageDifferenceType::EditTranslation;
    default:
        return LanguageDifferenceType::UnDefined;
    }
}
inline const int ConvertFromLanguageDifferenceType(const LanguageDifferenceType type)
{
    return static_cast<int>(type);
}
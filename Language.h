#include "Utility.h"
#include "Random.h"
#include <vector>
#include <string>
#include <map>

/**
 * @brief 音韻
 *
 */
struct Phomene
{
    // 調音方法
    int Manner;
    // 調音部位
    int Place;

    bool operator==(const Phomene &other) const
    {
        return (Manner == other.Manner && Place == other.Place);
    }

    bool operator!=(const Phomene &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Phomene &other) const
    {
        if (Manner != other.Manner)
            return Manner < other.Manner;
        return Place < other.Place;
    }
};

/**
 * @brief 意味ベクトル
 *
 */
class Meaning : public std::map<std::string, double>
{
public:
    Meaning Add(const Meaning &meaning) const;
    double Dot(const Meaning &meaning) const;
    Meaning Product(const double scalar) const;
    void Normalize();
};

struct Language;

/**
 * @brief 単語
 *
 */
struct Word
{
    // 発音
    std::vector<Phomene> Form;
    // 意味
    Meaning Meanings;
    // 意味が対応する祖語の単語
    std::vector<Phomene> ReconstructedWord;

    bool operator==(const Word &other) const
    {
        return Form == other.Form;
    }

    bool operator!=(const Word &other) const
    {
        return !operator==(other);
    }

    bool operator<(const Word &other) const
    {
        return Form < other.Form;
    }

    Word Add(const Word &word) const;
    void UpdateReconstructedWord(const Language &protoLanguage);
};

/**
 * @brief 言語
 *
 */
struct Language
{
    // 影響度、大きい方から小さいほうへ単語が借用される
    double Strength;
    // 語彙
    std::map<int, Word> Words;
};

/**
 * @brief 音韻変化の条件
 *
 */
enum PhoneticEnvironment
{
    // 語頭
    Start,
    // 語中
    Middle,
    // 語尾
    End
};

/**
 * @brief 音韻変化
 *
 */
struct PhonologicalChange
{
    // 変化前の音韻
    Phomene BeforePhoneme;
    // 条件
    PhoneticEnvironment PhoneticEnvironment;
    // 音韻が消えるか
    bool IsRemove;
    // 変化前の音韻
    Phomene AfterPhoneme;
};

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
    AddWord,
    // 影響度変化
    // string 地理
    // double 影響度
    ChangeStrength,
    // 音韻変化
    // string 地理
    // int 単語ID
    // PhonologicalChange 音韻変化
    PhonologicalChange,
    // 意味変化
    // string 地理
    // int 単語ID
    // Meaning 意味変化
    SemanticChange,
    // 借用
    // string 地理
    // int 借用元単語ID
    // string 地理
    // int 借用先単語ID
    Loanword,
    // 複合語
    // string 地理
    // int 単語ID
    // int... 参照単語ID
    AddCompound,
    // 死語
    // string 地理
    // int 単語ID
    ObsoleteWord
};

/**
 * @brief 語族差分
 *
 */
struct LanguageDifference
{
    // タイプ
    LanguageDifferenceType Type;
    // 時代
    int Period;
    // 整数パラメータ
    std::vector<int> IntParam;
    // 実数パラメータ
    std::vector<double> DoubleParam;
    // 文字列パラメータ
    std::vector<std::string> StringParam;
    // 音韻変化（あとで消す）
    PhonologicalChange PhonologicalChanges;
    // 意味変化（あとで消す）
    Meaning SemanticChange;

    static LanguageDifference CreateAddWord(const std::string &ID, const int period, const int wordID, const std::string &wordForm);
    static LanguageDifference CreateChangeStrength(const std::string &ID, const int period, const double strength);
    static LanguageDifference CreatePhonologicalChange(const std::string &ID, const int period, const int wordID, const PhonologicalChange phonologicalChange);
    static LanguageDifference CreateSemanticChange(const std::string &ID, const int period, const int wordID, const Meaning meaning);
    static LanguageDifference CreateLoanword(const std::string &ID1, const std::string &ID2, const int period, const int wordID1, const int wordID2);
    static LanguageDifference CreateAddCompound(const std::string &ID, const int period, const int wordID, const std::vector<int> wordIDs);
    static LanguageDifference CreateObsoleteWord(const std::string &ID, const int period, const int wordID);
};

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhonemeConverter
{
    std::map<std::string, Phomene> Map;
    PhonemeConverter static Create(const std::vector<std::vector<std::string>> &table);

    std::vector<Phomene> ConvertToPhoneme(const std::string &str);
    Language convertToLanguage(const std::vector<std::string> &strs);
};

/**
 * @brief 語族
 *
 */
struct LanguageFamily
{
    // 時代
    int Period = 0;
    // 地理
    std::vector<std::vector<std::string>> Geography;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeTable;
    // 地理と言語の対応（シミュレーション用）
    std::map<std::string, Language> Languages;
    // 祖語（シミュレーション用）
    Language ProtoLanguage;
    // 祖語からの差分
    std::vector<LanguageDifference> languageDifference;

    void SetProtoLanguageOnGeography(
        const std::string &startPlace,
        const Language &protoLanguage);
    std::vector<std::string> GetWords(std::string place);
    void PhonologicalChangeRandom(
        const double pPhonologicalChange,
        const double pSoundLoss,
        const bool isProhibitMinimalPair = true,
        const bool isSoundDuplication = true);
    void SemanticChangeRandom(
        const double pSemanticShift,
        const double maxSemanticShiftRate);
    void LoanwordRandom(const int nLoanword, const double pLoanword);
    void ChangeLanguageStrengthRandom(const double pChangeStrength);
    void ObsoleteWordRandom(const double pWordLoss);
    void MakeCompoundRandom(const double pWordBirth);
    void ExportLanguageToCSV(const std::string &filename);
    bool HasAllPlaceLanguage();
    void ToNextPeriod();
    void ApplyDifference(const LanguageDifference &diff);
    void ApplyDifferences(const std::vector<LanguageDifference> &diffs);
    void Export(const std::string &filename);
    void Import(const std::string &filename);
};

std::string convertToString(const std::vector<Phomene> &Phonemes, const std::vector<std::vector<std::string>> &table);
PhonologicalChange makepPhonologicalChangeRandom(const Phomene &beforePhoneme, const std::vector<std::vector<std::string>> &table, const double pRemovePhoneme);
Phomene getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table);
Phomene getRandomSoundFromLanguage(Language &language);
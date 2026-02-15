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
    /**
     * @brief 意味ベクトルの加算
     *
     * @param mean 可算する意味ベクトル
     */
    Meaning Add(const Meaning &meaning) const;

    /**
     * @brief 意味ベクトルの内積
     *
     * @param meaning 掛ける意味ベクトル
     */
    double Dot(const Meaning &meaning) const;

    /**
     * @brief 実数倍
     *
     * @param scalar 掛ける実数
     */
    Meaning Product(const double scalar) const;

    /**
     * @brief 正規化
     *
     */
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

    /**
     * @brief 複合語を生成
     *
     * @param word 単語
     * @return 複合語
     */
    Word Add(const Word &word) const;

    /**
     * @brief reconstructedWordを更新する
     *
     * @param protoLanguage 祖語
     */
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
    /**
     * @brief Create a Add 単語 object
     *
     * @param ID 言語ID
     * @param period 時代
     * @param wordID 単語ID
     * @param wordForm 語形
     * @return LanguageDifference
     */
    static LanguageDifference CreateAddWord(const std::string &ID, const int period, const int wordID, const std::string &wordForm);
    /**
     * @brief Change 言語 影響度
     *
     * @param ID 言語ID
     * @param period 時代
     * @param strength 影響度
     * @return LanguageDifference
     */
    static LanguageDifference CreateChangeStrength(const std::string &ID, const int period, const double strength);
    /**
     * @brief Change 言語 音韻
     *
     * @param ID 言語ID
     * @param period 時代
     * @param wordID 単語ID
     * @param phonologicalChange 音韻変化
     * @return LanguageDifference
     */
    static LanguageDifference CreatePhonologicalChange(const std::string &ID, const int period, const int wordID, const PhonologicalChange phonologicalChange);
    /**
     * @brief Change 単語の意味
     *
     * @param ID 言語ID
     * @param period 時代
     * @param wordID 単語ID
     * @param meaning 意味変化
     * @return LanguageDifference
     */
    static LanguageDifference CreateSemanticChange(const std::string &ID, const int period, const int wordID, const Meaning meaning);
    /**
     * @brief 借用
     *
     * @param ID1 借用元言語ID
     * @param ID2 借用先言語ID
     * @param period 時代
     * @param wordID1 借用元単語ID
     * @param wordID2 借用先単語ID
     * @return LanguageDifference
     */
    static LanguageDifference CreateLoanword(const std::string &ID1, const std::string &ID2, const int period, const int wordID1, const int wordID2);
    /**
     * @brief 複合語
     *
     * @param ID 言語ID
     * @param period 時代
     * @param wordID 単語ID
     * @param wordIDs 参照単語ID
     * @return LanguageDifference
     */
    static LanguageDifference CreateAddCompound(const std::string &ID, const int period, const int wordID, const std::vector<int> wordIDs);
    /**
     * @brief 単語削除
     *
     * @param ID 言語ID
     * @param period 時代
     * @param wordID 単語ID
     * @return LanguageDifference
     */
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

    /**
     * 文字列を変換表に基づいて音素列に変換する
     * @param str 文字列
     * @param table 音素表
     */
    std::vector<Phomene> ConvertToPhoneme(const std::string &str);

    /**
     * @brief 文字列の配列を言語に変換する
     * @param strs 文字列の配列
     * @param table 音素表
     * @return 言語
     */
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

    /**
     * 地図データの特定の位置に祖語を配置する
     * @param startPlace 祖語を配置する位置
     * @param protoLanguage 祖語
     */
    void SetProtoLanguageOnGeography(
        const std::string &startPlace,
        const Language &protoLanguage);

    /**
     * @brief Get the 単語 object
     *
     * @param place 位置
     * @return std::vector<std::string>
     */
    std::vector<std::string> GetWords(std::string place);

    /**
     * 音変化
     * @param pPhonologicalChange 音韻変化確率
     * @param pSoundLoss 音素脱落確率
     * @param isProhibitMinimalPair ミニマルペアを禁止するか
     * @param isSoundDuplication 音素の重複を禁止するか
     *
     * @note ある言語の単語を一斉に変化させる。
     */
    void PhonologicalChangeRandom(
        const double pPhonologicalChange,
        const double pSoundLoss,
        const bool isProhibitMinimalPair = true,
        const bool isSoundDuplication = true);

    /**
     * @brief 意味変化
     *
     * @param pSemanticShift 意味変化確率
     * @param maxChangeRate 意味変化大きさ
     *
     * @note 単語１つの意味を変化させる
     */
    void SemanticChangeRandom(
        const double pSemanticShift,
        const double maxSemanticShiftRate);

    /**
     * 単語を借用
     *
     * @param nLoanword 借用回数
     * @param pLoanword 借用率
     *
     * @note 借用の履歴をlanguageに記録
     */
    void LoanwordRandom(const int nLoanword, const double pLoanword);

    /**
     * @brief 言語の影響度をランダムに変化させる
     *
     * @param pChangeStrength 変化率
     */
    void ChangeLanguageStrengthRandom(const double pChangeStrength);

    /**
     * @brief 言語からランダムに単語を消去する
     *
     * @param pWordLoss 単語消去率
     */
    void ObsoleteWordRandom(const double pWordLoss);

    /**
     * @brief 言語に単語を追加する
     *
     * @param pWordBirth 単語追加律
     */
    void MakeCompoundRandom(const double pWordBirth);

    /**
     * Language構造体のリストをCSVに出力する
     * @param filename 出力ファイル名
     */
    void ExportLanguageToCSV(const std::string &filename);

    /**
     * @brief 各地に言語があるか
     *
     * @return true
     * @return false
     */
    bool HasAllPlaceLanguage();

    /**
     * @brief 時代を進める
     *
     */
    void ToNextPeriod();

    /**
     * @brief 差分を適用
     *
     * @param diff 差分
     */
    void ApplyDifference(const LanguageDifference &diff);

    /**
     * @brief 差分を複数適用
     *
     * @param diffs 差分
     */
    void ApplyDifferences(const std::vector<LanguageDifference> &diffs);

    /**
     * @brief 差分をファイル出力
     *
     */
    void Export(const std::string &filename);

    /**
     * @brief ファイル読み込み
     *
     * @param filename ファイルパス
     */
    void Import(const std::string &filename);
};

/**
 * 音素列を変換表に基づいて文字列に復元する
 * @param Phonemes 音素列
 * @param table 音素表
 */
std::string convertToString(const std::vector<Phomene> &Phonemes, const std::vector<std::vector<std::string>> &table);

/**
 * 変化規則をランダムに生成
 * @param beforePlace 変化前音素
 * @param beforeMannar 変化前音素
 * @param table 音素表
 * @param pRemovePhoneme 音が脱落する確率
 */
PhonologicalChange makepPhonologicalChangeRandom(const Phomene &beforePhoneme, const std::vector<std::vector<std::string>> &table, const double pRemovePhoneme);

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param table 音素表
 * @return 音素
 */
Phomene getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table);

/**
 * @brief 言語から、音素をランダムに1つ選択する
 * @param language 言語
 * @return 音素
 */
Phomene getRandomSoundFromLanguage(Language &language);

/**
 * @brief ファイルを読み込み差分を生成
 *
 * @param filename ファイルパス
 * @return std::vector<LanguageDifference>
 */
std::vector<LanguageDifference> ConvertToLanguageDifference(const std::string &filename);
#include "Utility.h"
#include "Random.h"
#include <vector>
#include <string>
#include <map>

/**
 * @brief 音韻
 *
 */
struct Phonetics
{
    // 調音方法
    int Mannar;
    // 調音部位
    int Place;

    bool operator==(const Phonetics &other) const
    {
        return (Mannar == other.Mannar && Place == other.Place);
    }

    bool operator!=(const Phonetics &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Phonetics &other) const
    {
        if (Mannar != other.Mannar)
            return Mannar < other.Mannar;
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
    std::vector<Phonetics> Sounds;
    // 意味
    Meaning Meanings;
    // 最も意味の近い祖語の単語
    // 高速化のためメンバ化
    std::vector<Phonetics> NearestProtoWord;

    bool operator==(const Word &other) const
    {
        return Sounds == other.Sounds;
    }

    bool operator!=(const Word &other) const
    {
        return !operator==(other);
    }

    bool operator<(const Word &other) const
    {
        return Sounds < other.Sounds;
    }

    /**
     * @brief 複合語を生成
     *
     * @param word 単語
     * @return 複合語
     */
    Word Add(const Word &word) const;

    /**
     * @brief NearestProtoWordを更新する
     *
     * @param language 祖語
     */
    void UpdateNearestProtoWord(const Language &language);
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
enum SoundChangeCondition
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
struct SoundChange
{
    // 変化前の音韻
    Phonetics beforePhon;
    // 条件
    SoundChangeCondition Condition;
    // 音韻が消えるか
    bool IsRemove;
    // 変化前の音韻
    Phonetics AfterPhone;
};

/**
 * @brief 語族差分タイプ
 *
 */
enum LanguageDifferenceType
{
    // 単語追加
    // int 地理ID
    // int 単語ID
    // string 語形
    // Meaning 意味
    AddWord,
    // 影響度変化
    // int 地理ID
    // double 影響度
    ChangeStrength,
    // 音韻変化
    // int 地理ID
    // int 単語ID
    // SoundChange 音韻変化
    ChangeSound,
    // 意味変化
    // int 地理ID
    // int 単語ID
    // Meaning 意味変化
    ChangeMeaning,
    // 借用
    // int 借用元地理ID
    // int 借用元単語ID
    // int 借用先地理ID
    // int 借用先単語ID
    BorrowWord,
    // 複合語
    // int 地理ID
    // int 単語ID
    // int... 参照単語ID
    AddCompoundWord,
    // 死語
    // int 地理ID
    // int 単語ID
    Remove
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
    int Section;
    // 整数パラメータ
    std::vector<int> IntParam;
    // 実数パラメータ
    std::vector<double> DoubleParam;
    // 文字列パラメータ
    std::vector<std::string> StringParam;
    // 音韻変化（あとで消す）
    SoundChange SoundChanges;
    // 意味変化（あとで消す）
    Meaning MeaningChange;
    /**
     * @brief Change 言語 影響度
     *
     * @param ID 言語ID
     * @param section 時代
     * @param strength 影響度
     * @return LanguageDifference
     */
    static LanguageDifference CreateChangeStrength(const int ID, const int section, const double strength);
    /**
     * @brief Change 言語 音韻
     *
     * @param ID 言語ID
     * @param section 時代
     * @param wordID 単語ID
     * @param soundChange 音韻変化
     * @return LanguageDifference
     */
    static LanguageDifference CreateChangeSound(const int ID, const int section, const int wordID, const SoundChange soundChange);
    /**
     * @brief Change 単語の意味
     *
     * @param ID 言語ID
     * @param section 時代
     * @param wordID 単語ID
     * @param meaning 意味変化
     * @return LanguageDifference
     */
    static LanguageDifference CreateChangeMeaning(const int ID, const int section, const int wordID, const Meaning meaning);
    /**
     * @brief 借用
     *
     * @param ID1 借用元言語ID
     * @param ID2 借用先言語ID
     * @param section 時代
     * @param wordID1 借用元単語ID
     * @param wordID2 借用先単語ID
     * @return LanguageDifference
     */
    static LanguageDifference CreateBorrowWord(const int ID1, const int ID2, const int section, const int wordID1, const int wordID2);
    /**
     * @brief 複合語
     *
     * @param ID 言語ID
     * @param section 時代
     * @param wordID 単語ID
     * @param wordIDs 参照単語ID
     * @return LanguageDifference
     */
    static LanguageDifference CreateAddCompoundWord(const int ID, const int section, const int wordID, const std::vector<int> wordIDs);
    /**
     * @brief 単語削除
     *
     * @param ID 言語ID
     * @param section 時代
     * @param wordID 単語ID
     * @return LanguageDifference
     */
    static LanguageDifference CreateRemoveWord(const int ID, const int section, const int wordID);
};

/**
 * @brief 語族
 *
 */
struct LanguageSystem
{
    // 地名
    std::vector<std::vector<std::string>> Map;
    // 音韻
    std::vector<std::vector<std::string>> PhonemeMap;
    // 言語
    std::map<int, Language> Languages;
};

/**
 * @brief 音素 <-> 表記変換
 *
 */
struct PhoneticsConverter
{
    std::map<std::string, Phonetics> Map;
    PhoneticsConverter static Create(const std::vector<std::vector<std::string>> &table);

    /**
     * 文字列を変換表に基づいて音素列に変換する
     * @param str 文字列
     * @param table 音素表
     */
    std::vector<Phonetics> convertToPhonetics(const std::string &str);

    /**
     * @brief 文字列の配列を言語に変換する
     * @param strs 文字列の配列
     * @param table 音素表
     * @return 言語
     */
    Language convertToLanguage(const std::vector<std::string> &strs);
};

/**
 * 地図データの特定の位置に祖語を配置する
 * @param mapData 地図データ
 * @param startPlace 祖語を配置する位置
 * @param language 祖語
 */
std::map<std::string, Language> setOldLanguageOnMap(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const Language &language);

/**
 * 音素列を変換表に基づいて文字列に復元する
 * @param phoneticses 音素列
 * @param table 音素表
 */
std::string convertToString(const std::vector<Phonetics> &phoneticses, const std::vector<std::vector<std::string>> &table);

/**
 * 音変化
 * @param language 言語
 * @param soundChange 変化規則
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 * @param isSoundDuplication 音素の重複を禁止するか
 *
 * @note ある言語の単語を一斉に変化させる。
 */
void changeLanguageSound(
    Language &language,
    const SoundChange &soundChange,
    const bool isProhibitMinimalPair = true,
    const bool isSoundDuplication = true);

/**
 * @brief 意味変化
 *
 * @param language 言語
 * @param oldLanguage 祖語
 * @param maxChangeRate 最大変化率
 *
 * @note 単語１つの意味を変化させる
 */
void changeLanguageMeaning(
    Language &language,
    const Language &oldLanguage,
    const double maxChangeRate);

/**
 * 変化規則をランダムに生成
 * @param beforePlace 変化前音素
 * @param beforeMannar 変化前音素
 * @param table 音素表
 * @param pRemoveSound 音が脱落する確率
 */
SoundChange makeSoundChangeRandom(const Phonetics &beforePhon, const std::vector<std::vector<std::string>> &table, const double pRemoveSound);

/**
 * Language構造体のリストをCSVに出力する
 * @param oldLanguage 祖語データ
 * @param languages 言語データ
 * @param table 音素表
 * @param filename 出力ファイル名
 */
void exportLanguageToCSV(
    Language &oldLanguage,
    const std::map<std::string, Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::wstring &filename);

/**
 * 単語を借用
 * @param languages 変化前音素
 * @param generation 世代
 * @param adjucentData 変化前音素
 *
 * @note 借用の履歴をlanguageに記録
 */
void bollowWord(std::map<std::string, Language> &languages, const int &generation, const std::pair<std::string, std::string> &adjucentData);

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param table 音素表
 * @return 音素
 */
Phonetics getRandomSoundFromTable(const std::vector<std::vector<std::string>> &table);

/**
 * @brief 言語から、音素をランダムに1つ選択する
 * @param language 言語
 * @return 音素
 */
Phonetics getRandomSoundFromLanguage(Language &language);

/**
 * @brief 言語の影響度をランダムに変化させる
 * @param language 言語
 */
void changeLanguageStrength(Language &language);

/**
 * @brief 言語からランダムに単語を消去する
 *
 * @param language 言語
 */
void removeWordRandom(Language &language, const Language &oldLanguage);

/**
 * @brief 言語に単語を追加する
 *
 * @param language 言語
 * @param oldLanguage 祖語
 */
void createWord(Language &language, const Language &oldLanguage);
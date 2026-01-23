#include "Utility.h"
#include "Random.h"
#include <vector>
#include <string>

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
 * @brief 言語
 *
 */
struct Language
{
    // 位置
    std::string Place;
    // 影響度、大きい方から小さいほうへ単語が借用される
    double Strength;
    // 言語
    std::vector<std::vector<Phonetics>> Lang;
    // 借用履歴
    std::vector<std::pair<int, std::string>> BollowHistory;
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
 * 文字列を変換表に基づいて音素列に変換する
 * @param str 文字列
 * @param table 音素表
 */
std::vector<Phonetics> convertToPhonetics(const std::string &str, const std::vector<std::vector<std::string>> &table);

/**
 * 地図データの特定の位置に祖語を配置する
 * @param mapData 地図データ
 * @param startPlace 祖語を配置する位置
 * @param language 祖語
 */
std::vector<Language> createConditionalPairs(
    const std::vector<std::string> &mapData,
    const std::string &startPlace,
    const std::vector<std::vector<Phonetics>> &language);

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
 */
void changeLanguageSound(
    Language &language,
    const SoundChange &soundChange,
    const bool isProhibitMinimalPair = true,
    const bool isSoundDuplication = true);

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
void exportLanguageStructToCSV(
    const std::vector<std::vector<Phonetics>> &oldLanguage,
    const std::vector<struct Language> &languages,
    const std::vector<std::vector<std::string>> &table,
    const std::string &filename);

/**
 * 単語を借用
 * @param languages 変化前音素
 * @param generation 世代
 * @param adjucentData 変化前音素
 *
 * @note 借用の履歴をlanguageに記録
 */
void BollowWord(std::vector<Language> &languages, const int &generation, const std::pair<std::string, std::string> &adjucentData);

/**
 * @brief 音素表から、音素をランダムに1つ選択する
 * @param table 音素表
 * @return 音素
 */
Phonetics getRandomNonEmptyIndex(const std::vector<std::vector<std::string>> &table);

/**
 * @brief 言語から、音素をランダムに1つ選択する
 * @param language 言語
 * @return 音素
 */
Phonetics getRandomNonEmptyIndex(const Language &language);

/**
 * @brief 言語の影響度をランダムに変化させる
 * @param language 言語
 */
void changeLanguageStrength(Language &language);
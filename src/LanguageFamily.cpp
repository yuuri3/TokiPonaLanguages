#include "LanguageFamily.h"
#include "PhonemeConverter.h"
#include "Language.h"
#include "Utility.h"

namespace
{

    /**
     * @brief YAML読み込み
     *
     * @param file
     * @param line
     * @return true
     * @return false
     */
    bool ImportYAML(std::ifstream &file, std::vector<std::string> &geoLine)
    {
        std::string line;
        if (!std::getline(file, line))
        {
            return false;
        }
        geoLine = ParseVector(line);
        if (geoLine.empty())
        {
            return false;
        }
        return true;
    }
}

/**
 * @brief ラテンアルファベットの表
 *
 */
std::vector<std::vector<std::string>> LanguageFamily::RomanAlphabetTable =
    {{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"}};

/**
 * @brief 空の語族を作成
 *
 * @param geography 地理
 * @param phonemeTable 音韻
 * @return LanguageFamily
 */
LanguageFamily LanguageFamily::Create(const std::vector<std::vector<std::string>> &geography, const std::vector<std::vector<std::string>> &phonemeTable)
{
    LanguageFamily languages;
    languages.languageDifference_ = {};
    languages.Geography_ = geography;
    languages.PhonemeTable_ = phonemeTable;
    return languages;
}

/**
 * @brief 地理を取得
 *
 * @return const std::vector<std::vector<std::string>>
 */
const std::vector<std::vector<std::string>> &LanguageFamily::GetGeography() const
{
    return Geography_;
}

/**
 * @brief 音韻を取得
 *
 * @return const std::vector<std::vector<std::string>>
 */
const std::vector<std::vector<std::string>> &LanguageFamily::GetPhonemeTable() const
{
    return PhonemeTable_;
}

/**
 * @brief 差分を追加
 *
 * @param languageDifference
 */
void LanguageFamily::AddDifference(const LanguageDifference &languageDifference)
{
    languageDifference_.emplace_back(languageDifference);
}

/**
 * @brief 地名を変更
 *
 * @param row 行
 * @param column 列
 * @param name 地名
 */
void LanguageFamily::ChangePlaceName(const int row, const int column, const std::string &name)
{
    if (row < 0 || row >= Geography_.size())
    {
        return;
    }
    if (column < 0 || column >= Geography_[row].size())
    {
        for (int i = 0; i < Geography_[row].size() - column; i++)
        {
            Geography_[row].emplace_back("");
        }
        Geography_[row].emplace_back(name);
    }
    else
    {
        Geography_[row][column] = name;
    }
}

/**
 * @brief 上行に地理を追加
 *
 */
void LanguageFamily::AddGeomgraphicRowAbove(const int row)
{
    if (row < 0 || row >= Geography_.size())
    {
        return;
    }
    Geography_.insert(Geography_.begin() + row, std::vector<std::string>());
}

/**
 * @brief 下行に地理を追加
 *
 */
void LanguageFamily::AddGeomgraphicRowBelow(const int row)
{
    if (row < 0 || row >= Geography_.size())
    {
        return;
    }
    Geography_.insert(Geography_.begin() + row + 1, std::vector<std::string>());
}

/**
 * @brief 地理行を削除
 *
 */
void LanguageFamily::DeleteGeomgraphicRow(const int row)
{
    if (row < 0 || row >= Geography_.size())
    {
        return;
    }
    Geography_.erase(Geography_.begin() + row);
}

/**
 * @brief 右列に地理を追加
 *
 * @param column
 */
void LanguageFamily::AddGeomgraphicColumnRight(const int column)
{
    for (auto &line : Geography_)
    {
        if (column < 0 || column >= line.size())
        {
            continue;
        }
        line.insert(line.begin() + column + 1, "");
    }
}

/**
 * @brief 左列に地理を追加
 *
 * @param column
 */
void LanguageFamily::AddGeomgraphicColumnLeft(const int column)
{
    for (auto &line : Geography_)
    {
        if (column < 0 || column >= line.size())
        {
            continue;
        }
        line.insert(line.begin() + column, "");
    }
}

/**
 * @brief 地理の列を削除
 *
 * @param column
 */
void LanguageFamily::DeleteGeomgraphicColumn(const int column)
{
    for (auto &line : Geography_)
    {
        if (column < 0 || column >= line.size())
        {
            continue;
        }
        line.erase(line.begin() + column);
    }
}

/**
 * @brief 上に時代を追加
 *
 * @param period
 */
void LanguageFamily::AddPeriodAbove(const int period)
{
    for (auto &diff : languageDifference_)
    {
        if (diff.GetPeriod() >= period)
        {
            diff.AddPeriod();
        }
    }
}

/**
 * @brief 上に時代を追加
 *
 * @param period
 */
void LanguageFamily::AddPeriodBelow(const int period)
{
    for (auto &diff : languageDifference_)
    {
        if (diff.GetPeriod() > period)
        {
            diff.AddPeriod();
        }
    }
}

/**
 * @brief 時代を削除
 *
 * @param period
 */
void LanguageFamily::RemovePeriod(const int period)
{
    languageDifference_.erase(
        std::remove_if(languageDifference_.begin(), languageDifference_.end(),
                       [&](const LanguageDifference &diff)
                       { return diff.GetPeriod() == period; }),
        languageDifference_.end());

    for (auto &diff : languageDifference_)
    {
        if (diff.GetPeriod() > period)
        {
            diff.SubPeriod();
        }
    }
}

/**
 * @brief 語族に情報が不足しているか
 *
 * @return true
 * @return false
 */
const bool LanguageFamily::Empty() const
{
    return Geography_.empty() || PhonemeTable_.empty();
}

/**
 * @brief 言語を計算
 *
 * @param place 位置
 * @param period 時代
 * @return std::optional<Language>
 */
std::optional<Language> LanguageFamily::CalculateLanguage(const std::string place, const int period)
{
    std::map<std::string, Language> languages;
    const auto converter = PhonemeConverter::Create(PhonemeTable_);

    for (const auto &diff : languageDifference_)
    {
        if (diff.GetPeriod() > period)
        {
            return languages.at(place);
        }
        if (!LanguageUtility::ApplyDifference(diff, languages, converter))
        {
            return std::nullopt;
        }
    }
    return languages.at(place);
}

/**
 * @brief 言語名の配列を出力
 *
 * @return const std::vector<std::vector<std::string>>
 */
const std::vector<std::vector<std::string>> LanguageFamily::ToString() const
{
    int currentPeriod = 0;
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;
    int period = 0;
    std::map<std::string, Language> languages;

    auto converter = PhonemeConverter::Create(GetPhonemeTable());

    for (const auto &place : getNonEmptyStrings(GetGeography()))
    {
        line.emplace_back(place);
    }
    result.emplace_back(line);
    line.clear();

    for (const auto &diff : languageDifference_)
    {
        while (diff.GetPeriod() > currentPeriod)
        {
            currentPeriod++;
            for (const auto &place : getNonEmptyStrings(GetGeography()))
            {
                if (languages.count(place) == 0 || languages[place].Empty())
                {
                    line.emplace_back("");
                }
                else
                {
                    line.emplace_back(converter.ConvertToString(languages[place].GetWord(0)->GetForm()));
                }
            }
            result.emplace_back(line);
            line.clear();
        }
        if (!LanguageUtility::ApplyDifference(diff, languages, converter))
        {
            return {};
        }
    }

    for (const auto &place : getNonEmptyStrings(GetGeography()))
    {
        if (languages.count(place) == 0 || languages[place].Empty())
        {
            line.emplace_back("");
        }
        else
        {
            line.emplace_back(converter.ConvertToString(languages[place].GetWord(0)->GetForm()));
        }
    }
    result.emplace_back(line);
    line.clear();

    return result;
}

/**
 * @brief 差分をファイル出力
 *
 */
void LanguageFamily::Export(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return;

    // 2. Map
    file << "Map:\n";
    for (const auto &row : Geography_)
    {
        file << FormatVector(row) << "\n";
    }

    // 3. PhoneticsMap
    file << "PhoneticsMap:\n";
    for (const auto &row : PhonemeTable_)
    {
        file << FormatVector(row) << "\n";
    }

    // 4. LanguageDifferences
    file << "LanguageDifferences:\n";
    for (const auto &diff : languageDifference_)
    {
        diff.Export(file);
    }

    file.close();
}

/**
 * @brief ファイル読み込み
 *
 * @param filename ファイルパス
 *
 * @return 読み込みの成否
 */
bool LanguageFamily::Import(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    std::string line;

    // Map
    if (!std::getline(file, line) || line != "Map:")
        return false;
    Geography_.clear();
    while (std::getline(file, line) && line != "PhoneticsMap:")
    {
        Geography_.emplace_back(ParseVector(line));
    }
    // PhoneticsMap
    PhonemeTable_.clear();
    while (std::getline(file, line) && line != "LanguageDifferences:")
    {
        PhonemeTable_.emplace_back(ParseVector(line));
    }
    // LanguageDifferences
    languageDifference_.clear();
    LanguageDifference dif;
    while (LanguageDifference::Import(file, dif))
    {
        languageDifference_.emplace_back(dif);
    }
    // 途中で終了した場合、読み込み失敗とする
    if (std::getline(file, line))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

/**
 * @brief json ファイル読み込み
 *
 * @param filename ファイル名
 * @return true
 * @return false
 */
bool LanguageFamily::ImportJson(const std::string &filename)
{
    // 地理データ
    Geography_ = {{"0"}};

    // 音韻データ
    PhonemeTable_ = RomanAlphabetTable;

    // 言語
    const auto data = ImportFromJson(QString::fromStdString(filename));
    if (!data.success)
    {
        return false;
    }

    // 単語追加
    int wordID = 0;
    const auto converter = PhonemeConverter::Create(RomanAlphabetTable);
    for (const auto &qWord : data.words)
    {
        const auto word = Word::CreateFromJsonObject(qWord.toObject(), converter);
        auto dif = LanguageDifference::CreateAddWord("0", 0, wordID, converter.ConvertToString(word.GetForm()));
        languageDifference_.emplace_back(dif);

        for (const auto partID : word.GetPartIDs())
        {
            const auto part = word.GetPart(partID);
            dif = LanguageDifference::CreateEditPart("0", 0, wordID, partID, part);
            languageDifference_.emplace_back(dif);
            for (const auto translationID : word.GetTranslationIDs(partID))
            {
                const auto translation = word.GetTranslation(partID, translationID);
                dif = LanguageDifference::CreateEditTranslation("0", 0, wordID, partID, translationID, translation);
                languageDifference_.emplace_back(dif);
            }
        }
        int tagID = 0;
        for (const int tagID : word.GetTagIDs())
        {
            const auto tag = word.GetTag(tagID);
            dif = LanguageDifference::CreateEditTag("0", 0, wordID, tagID, tag);
        }
        wordID++;
    }

    return true;
}
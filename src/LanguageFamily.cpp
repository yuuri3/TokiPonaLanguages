#include "LanguageFamily.h"
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
LanguageFamily LanguageFamily::Create(const std::vector<std::vector<std::string>> &geography, const PhonemeTable &phonemeTable)
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
const PhonemeTable &LanguageFamily::GetPhonemeTable() const
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
    return Geography_.empty() || PhonemeTable_.Empty();
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

    for (const auto &diff : languageDifference_)
    {
        if (diff.GetPeriod() > period)
        {
            return languages.at(place);
        }
        if (!LanguageUtility::ApplyDifference(diff, languages, this))
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
                if (languages.count(place) == 0 || languages.find(place) == languages.end() || languages.at(place).Empty())
                {
                    line.emplace_back("");
                }
                else
                {
                    line.emplace_back(PhonemeTable_.ConvertToString(languages.at(place).GetWord(0)->GetForm()));
                }
            }
            result.emplace_back(line);
            line.clear();
        }
        if (!LanguageUtility::ApplyDifference(diff, languages, this))
        {
            return {};
        }
    }

    for (const auto &place : getNonEmptyStrings(GetGeography()))
    {
        if (languages.count(place) == 0 || languages.find(place) == languages.end() || languages.at(place).Empty())
        {
            line.emplace_back("");
        }
        else
        {
            line.emplace_back(PhonemeTable_.ConvertToString(languages.at(place).GetWord(0)->GetForm()));
        }
    }
    result.emplace_back(line);
    line.clear();

    return result;
}

/**
 * @brief 差分をファイル出力
 *
 * @param filename 出力ファイル名
 */
void LanguageFamily::Export(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return;
    }

    // 2. Map (地理情報)
    file << "Map:\n";
    file << Geography_.size() << "\n";
    for (const auto &row : Geography_)
    {
        file << FormatVector<std::string>(row) << "\n";
    }

    // 3. PhonemeTable (音韻マスタデータ)
    // 旧 PhoneticsMap セクションは、詳細なマスタデータ構造へ置き換え
    file << "PhonemeTable:\n";
    PhonemeTable_.Export(file);

    // 4. LanguageDifferences (言語変化の記録)
    file << "LanguageDifferences:\n";
    file << languageDifference_.size() << "\n";
    for (const auto &diff : languageDifference_)
    {
        diff.Export(file);
    }

    file.close();
}

/**
 * @brief 差分（および音韻マスタ・地図）をファイル読み込み
 *
 * @param filename 入力ファイル名
 * @return bool 成功したか
 */
bool LanguageFamily::Import(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    try
    {
        while (std::getline(file, line))
        {
            if (line == "Map:")
            {
                if (!std::getline(file, line))
                    return false;

                int rowCount = std::stoi(line);
                if (rowCount < 0)
                    return false; // 負の値チェック

                Geography_.clear();
                Geography_.reserve(rowCount); // 速度改善：メモリ予約

                for (int i = 0; i < rowCount; ++i)
                {
                    if (!std::getline(file, line))
                        return false;
                    Geography_.push_back(ParseVector(line));
                }
            }
            else if (line == "PhonemeTable:")
            {
                if (!PhonemeTable_.Import(file))
                {
                    return false;
                }
            }
            else if (line == "LanguageDifferences:")
            {
                if (!std::getline(file, line))
                    return false;

                int diffCount = std::stoi(line);
                if (diffCount < 0)
                    return false;

                languageDifference_.clear();
                languageDifference_.reserve(diffCount); // 速度改善：メモリ予約

                for (int i = 0; i < diffCount; ++i)
                {
                    LanguageDifference diff;
                    if (!LanguageDifference::Import(file, diff))
                    {
                        return false;
                    }
                    languageDifference_.push_back(std::move(diff));
                }
            }
        }
    }
    catch (const std::exception &)
    {
        // std::stoi 等の例外をキャッチして安全に中断
        return false;
    }

    file.close();
    return true;
}

/**
 * @brief json ファイル読み込み
 *
 * @param filename ファイル名
 * @return bool 成功したか
 */
bool LanguageFamily::ImportJson(const std::string &filename)
{
    // 地理データ初期化
    Geography_ = {{"0"}};

    // 音韻データ初期化
    PhonemeTable_ = PhonemeTable::CreateDummyTable();

    languageDifference_.clear();

    // 言語データロード
    const auto data = ImportFromJson(QString::fromStdString(filename));
    if (!data.success)
    {
        return false;
    }

    // 速度改善：単語数に応じてあらかじめメモリを予約（ネスト分も考慮して多めに確保）
    languageDifference_.reserve(data.words.size() * 5);

    int wordID = 0;
    for (const auto &qWord : data.words)
    {
        const auto word = Word::CreateFromJsonObject(qWord.toObject(), PhonemeTable_);

        // 単語追加
        languageDifference_.emplace_back(
            LanguageDifference::CreateAddWord("0", 0, wordID, word.GetForm()));

        // 各要素の編集差分を追加
        for (const auto partID : word.GetPartIDs())
        {
            languageDifference_.emplace_back(
                LanguageDifference::CreateEditPart("0", 0, wordID, partID, word.GetPart(partID)));

            for (const auto translationID : word.GetTranslationIDs(partID))
            {
                languageDifference_.emplace_back(
                    LanguageDifference::CreateEditTranslation("0", 0, wordID, partID, translationID, word.GetTranslation(partID, translationID)));
            }
        }

        for (const int tagID : word.GetTagIDs())
        {
            languageDifference_.emplace_back(
                LanguageDifference::CreateEditTag("0", 0, wordID, tagID, word.GetTag(tagID)));
        }

        for (const int contentID : word.GetContentIDs())
        {
            languageDifference_.emplace_back(
                LanguageDifference::CreateEditContent("0", 0, wordID, contentID, word.GetContentTitle(contentID), word.GetContent(contentID)));
        }

        for (const int variationID : word.GetVariationIDs())
        {
            languageDifference_.emplace_back(
                LanguageDifference::CreateEditVariation("0", 0, wordID, variationID, word.GetVariationTitle(variationID), word.GetVariation(variationID)));
        }

        for (const int relationID : word.GetRelationIDs())
        {
            languageDifference_.emplace_back(
                LanguageDifference::CreateSetRelation("0", 0, wordID, relationID, word.GetRelationTitle(relationID), word.GetRelationWordID(relationID)));
        }

        wordID++;
    }

    return true;
}
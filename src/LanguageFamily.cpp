#include "..\\include\LanguageFamily.h"

namespace
{
    // ヘルパー関数：ベクトルをカンマ区切りのリスト形式にする
    template <typename T>
    std::string formatYamlList(const std::vector<T> &vec)
    {
        if (vec.empty())
            return "[]";
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ss << vec[i] << (i == vec.size() - 1 ? "" : ", ");
        }
        ss << "]";
        return ss.str();
    }

    // YAMLの [a, b, c] 形式を vector<string> に変換する
    std::vector<std::string> parseYamlList(const std::string &line)
    {
        std::vector<std::string> result;
        size_t openingParenthesisPosition = line.find('[');
        size_t closingParenthesisPosition = line.find(']');
        if (openingParenthesisPosition == std::string::npos || closingParenthesisPosition == std::string::npos)
            return result;

        std::string content = line.substr(openingParenthesisPosition + 1, closingParenthesisPosition - openingParenthesisPosition - 1);
        std::stringstream ss(content);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            result.push_back(EraseSpace(item));
        }
        return result;
    }

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
        geoLine = parseYamlList(line);
        if (geoLine.empty())
        {
            return false;
        }
        return true;
    }
}

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
const std::vector<std::vector<std::string>> LanguageFamily::GetGeography() const
{
    return Geography_;
}

/**
 * @brief 音韻を取得
 *
 * @return const std::vector<std::vector<std::string>>
 */
const std::vector<std::vector<std::string>> LanguageFamily::GetPhonemeTable() const
{
    return PhonemeTable_;
}

/**
 * @brief 差分を出力
 *
 * @return std::vector<LanguageDifference>
 */
const std::vector<LanguageDifference> LanguageFamily::GetDifference() const
{
    return languageDifference_;
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
        file << "  - " << formatYamlList(row) << "\n";
    }

    // 3. PhoneticsMap
    file << "PhoneticsMap:\n";
    for (const auto &row : PhonemeTable_)
    {
        file << "  - " << formatYamlList(row) << "\n";
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

    enum Mode
    {
        Mode_Map,
        Mode_PhonemeTable,
        Mode_LanguageDifferences,
    };

    Mode mode;
    LanguageDifference dif;
    bool isDifferenceSection = false;

    std::string line;
    try
    {
        if (!std::getline(file, line) || line != "Map:")
        {
            return false;
        }
        // 地理
        {
            Geography_.clear();
            std::vector<std::string> geoLine;
            while (ImportYAML(file, geoLine))
            {
                Geography_.emplace_back(geoLine);
            }
        }
        // 音韻
        {
            PhonemeTable_.clear();
            std::vector<std::string> phonLine;
            while (ImportYAML(file, phonLine))
            {
                PhonemeTable_.emplace_back(phonLine);
            }
        }
        // 差分
        {
            languageDifference_.clear();
            LanguageDifference dif;
            while (LanguageDifference::Import(file, dif))
            {
                languageDifference_.emplace_back(dif);
            }
        }
    }
    catch (...)
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}
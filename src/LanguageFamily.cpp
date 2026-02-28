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
        while (std::getline(file, line))
        {
            if (line == "Map:")
            {
                mode = Mode::Mode_Map;
                Geography_.clear();
                continue;
            }
            else if (line == "PhoneticsMap:")
            {
                mode = Mode::Mode_PhonemeTable;
                PhonemeTable_.clear();
                continue;
            }
            else if (line == "LanguageDifferences:")
            {
                mode = Mode::Mode_LanguageDifferences;
                languageDifference_.clear();
                LanguageDifference dif;
                while (LanguageDifference::Import(file, dif))
                {
                    languageDifference_.emplace_back(dif);
                }
            }

            if (mode == Mode::Mode_Map)
            {
                Geography_.emplace_back(parseYamlList(line));
            }
            else if (mode == Mode::Mode_PhonemeTable)
            {
                PhonemeTable_.emplace_back(parseYamlList(line));
            }
            else
            {
                file.close();
                return false;
            }
        }
    }
    catch (...)
    {
        file.close();
        return false;
    }
    languageDifference_.emplace_back(dif);
    file.close();
    return true;
}
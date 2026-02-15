#include "LanguageFamily.h"
#include <fstream>
#include <sstream>

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

    // 前後のスペースを除去。 スペースのみのセルは空文字列とする。
    std::string EraseSpace(std::string str)
    {
        size_t first = str.find_first_not_of(" ");
        size_t last = str.find_last_not_of(" ");

        if (first != std::string::npos)
            return str.substr(first, (last - first + 1));
        else
            return "";
    }

    // YAMLの [a, b, c] 形式を vector<string> に変換する
    std::vector<std::string> parseYamlList(const std::string &line)
    {
        std::vector<std::string> result;
        size_t start = line.find('[');
        size_t end = line.find(']');
        if (start == std::string::npos || end == std::string::npos)
            return result;

        std::string content = line.substr(start + 1, end - start - 1);
        std::stringstream ss(content);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            result.push_back(EraseSpace(item));
        }
        return result;
    }

    /**
     * @brief 文字列を':'で前後2つに分割する
     * @return {前部分, 後部分}。コロンがない場合は {元の文字列, ""}
     */
    std::pair<std::string, std::string> splitByColon(const std::string &line)
    {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
        {
            return {line, ""};
        }

        std::string first = line.substr(0, pos);
        std::string second = line.substr(pos + 1);

        // 必要に応じて前後の空白を削除（トリミング）
        auto trim = [](std::string &s)
        {
            size_t f = s.find_first_not_of(" ");
            size_t l = s.find_last_not_of(" \r\n\t");
            if (f == std::string::npos)
                s = "";
            else
                s = s.substr(f, l - f + 1);
        };

        trim(first);
        trim(second);

        return {first, second};
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
    for (const auto &row : Geography)
    {
        file << "  - " << formatYamlList(row) << "\n";
    }

    // 3. PhoneticsMap
    file << "PhoneticsMap:\n";
    for (const auto &row : PhonemeTable)
    {
        file << "  - " << formatYamlList(row) << "\n";
    }

    // 4. LanguageDifferences
    file << "LanguageDifferences:\n";
    for (const auto &diff : languageDifference)
    {
        file << "  - Section: " << diff.Period << "\n";
        file << "    Type: " << static_cast<int>(diff.Type) << "\n";

        file << "    IntParam:\n";
        for (const auto &i : diff.IntParam)
            file << "      - " << i << "\n";

        file << "    DoubleParam:\n";
        for (const auto &d : diff.DoubleParam)
            file << "      - " << d << "\n";

        file << "    StringParam:\n";
        for (const auto &s : diff.StringParam)
            file << "      - " << s << "\n";

        file << "    SoundChange:\n";
        file << "      Before:\n";
        file << "        Place: " << diff.PhonologicalChanges.BeforePhoneme.Place << "\n";
        file << "        Mannar: " << diff.PhonologicalChanges.BeforePhoneme.Manner << "\n";
        file << "      After:\n";
        file << "        Place: " << diff.PhonologicalChanges.AfterPhoneme.Place << "\n";
        file << "        Mannar: " << diff.PhonologicalChanges.AfterPhoneme.Manner << "\n";
        file << "      Condition: " << static_cast<int>(diff.PhonologicalChanges.PhoneticEnvironment) << "\n";
        file << "      IsRemove: " << diff.PhonologicalChanges.IsRemove << "\n";

        file << "    MeaningChange:\n";
        for (const auto &pair : diff.SemanticChange)
        {
            file << "      - Key: " << pair.first << "\n";
            file << "        Value: " << pair.second << "\n";
        }
    }
}

/**
 * @brief ファイル読み込み
 *
 * @param filename ファイルパス
 */
void LanguageFamily::Import(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    enum Mode
    {
        Map_,
        PhonemeTable_,
        LanguageDifferences_,
    };

    enum SubMode
    {
        Type_,
        Period_,
        IntParam_,
        DoubleParam_,
        StringParam_,
        PhonologicalChanges_,
        SemanticChange_,
    };

    Mode mode;
    SubMode subMode;
    LanguageDifference dif;
    bool b = false;

    std::string line;
    while (std::getline(file, line))
    {
        if (line == "Map:")
        {
            mode = Mode::Map_;
            Geography.clear();
            continue;
        }
        else if (line == "PhoneticsMap:")
        {
            mode = Mode::PhonemeTable_;
            PhonemeTable.clear();
            continue;
        }
        else if (line == "LanguageDifferences:")
        {
            mode = Mode::LanguageDifferences_;
            languageDifference.clear();
            continue;
        }

        if (mode == Mode::Map_)
        {
            Geography.emplace_back(parseYamlList(line));
        }
        else if (mode == Mode::PhonemeTable_)
        {
            PhonemeTable.emplace_back(parseYamlList(line));
        }
        else if (mode == Mode::LanguageDifferences_)
        {
            auto [key, value] = splitByColon(line);
            if (key == "- Section")
            {
                if (b)
                {
                    languageDifference.emplace_back(dif);
                    dif = LanguageDifference();
                }
                else
                {
                    b = true;
                }
                dif.Period = std::stoi(value);
                continue;
            }
            else if (key == "Type")
            {
                dif.Type = static_cast<LanguageDifferenceType>(std::stoi(value));
                continue;
            }
            else if (line == "    IntParam:")
            {
                subMode = SubMode::IntParam_;
                continue;
            }
            else if (line == "    DoubleParam:")
            {
                subMode = SubMode::DoubleParam_;
                continue;
            }
            else if (line == "    StringParam:")
            {
                subMode = SubMode::StringParam_;
                continue;
            }
            else if (line == "    SoundChange:")
            {
                std::getline(file, line); // Before:
                std::getline(file, line); // Place:
                std::tie(key, value) = splitByColon(line);
                const int beforePlace = std::stoi(value);
                std::getline(file, line); // Manner:
                std::tie(key, value) = splitByColon(line);
                const int beforeManner = std::stoi(value);

                std::getline(file, line); // After:
                std::getline(file, line); // Place:
                std::tie(key, value) = splitByColon(line);
                const int afterPlace = std::stoi(value);
                std::getline(file, line); // Manner:
                std::tie(key, value) = splitByColon(line);
                const int afterManner = std::stoi(value);

                std::getline(file, line); // Condition:
                std::tie(key, value) = splitByColon(line);
                const auto phoneticEnvironment = static_cast<PhoneticEnvironment>(std::stoi(value));

                std::getline(file, line); // IsRemove:
                std::tie(key, value) = splitByColon(line);
                const bool isRemove = static_cast<bool>(std::stoi(value));

                dif.PhonologicalChanges.BeforePhoneme.Place = beforePlace;
                dif.PhonologicalChanges.BeforePhoneme.Manner = beforeManner;
                dif.PhonologicalChanges.AfterPhoneme.Place = afterPlace;
                dif.PhonologicalChanges.AfterPhoneme.Manner = afterManner;
                dif.PhonologicalChanges.PhoneticEnvironment = phoneticEnvironment;
                dif.PhonologicalChanges.IsRemove = isRemove;
                continue;
            }
            else if (line == "    MeaningChange:")
            {
                subMode = SubMode::SemanticChange_;
                continue;
            }

            if (subMode == SubMode::IntParam_)
            {
                dif.IntParam.emplace_back(std::stoi(line.substr(8)));
            }
            else if (subMode == SubMode::DoubleParam_)
            {
                dif.DoubleParam.emplace_back(std::stod(line.substr(8)));
            }
            else if (subMode == SubMode::StringParam_)
            {
                dif.StringParam.emplace_back(line.substr(8));
            }
            else if (subMode == SubMode::SemanticChange_)
            {
                std::getline(file, line);
                const auto [_1, key2] = splitByColon(line);
                std::getline(file, line);
                const auto [_2, value2] = splitByColon(line);
                dif.SemanticChange[key2] = std::stod(value2);
            }
        }
    }
    languageDifference.emplace_back(dif);
}
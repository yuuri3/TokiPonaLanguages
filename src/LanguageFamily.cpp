#include "..\\include\LanguageFamily.h"
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
        size_t firstNoSpacePosotion = str.find_first_not_of(" ");
        size_t lastNoSpacePosition = str.find_last_not_of(" ");

        if (firstNoSpacePosotion != std::string::npos)
            return str.substr(firstNoSpacePosotion, (lastNoSpacePosition - firstNoSpacePosotion + 1));
        else
            return "";
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
     * @brief 文字列を':'で前後2つに分割する
     * @return {前部分, 後部分}。コロンがない場合は {元の文字列, ""}
     */
    std::pair<std::string, std::string> splitByColon(const std::string &line)
    {
        size_t colonPosition = line.find(':');
        if (colonPosition == std::string::npos)
        {
            return {line, ""};
        }

        std::string firstItem = line.substr(0, colonPosition);
        std::string secondItem = line.substr(colonPosition + 1);

        return {EraseSpace(firstItem), EraseSpace(secondItem)};
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
    };

    Mode mode;
    SubMode subMode;
    LanguageDifference dif;
    bool isDifferenceSection = false;

    std::string line;
    try
    {
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
                auto [memberName, memberValue] = splitByColon(line);
                if (memberName == "- Section")
                {
                    if (isDifferenceSection)
                    {
                        languageDifference.emplace_back(dif);
                        dif = LanguageDifference();
                    }
                    else
                    {
                        isDifferenceSection = true;
                    }
                    dif.Period = std::stoi(memberValue);
                    continue;
                }
                else if (memberName == "Type")
                {
                    dif.Type = static_cast<LanguageDifferenceType>(std::stoi(memberValue));
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
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const int beforePlace = std::stoi(memberValue);
                    std::getline(file, line); // Manner:
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const int beforeManner = std::stoi(memberValue);

                    std::getline(file, line); // After:
                    std::getline(file, line); // Place:
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const int afterPlace = std::stoi(memberValue);
                    std::getline(file, line); // Manner:
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const int afterManner = std::stoi(memberValue);

                    std::getline(file, line); // Condition:
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const auto phoneticEnvironment = static_cast<PhoneticEnvironment>(std::stoi(memberValue));

                    std::getline(file, line); // IsRemove:
                    std::tie(memberName, memberValue) = splitByColon(line);
                    const bool isRemove = static_cast<bool>(std::stoi(memberValue));

                    dif.PhonologicalChanges.BeforePhoneme.Place = beforePlace;
                    dif.PhonologicalChanges.BeforePhoneme.Manner = beforeManner;
                    dif.PhonologicalChanges.AfterPhoneme.Place = afterPlace;
                    dif.PhonologicalChanges.AfterPhoneme.Manner = afterManner;
                    dif.PhonologicalChanges.PhoneticEnvironment = phoneticEnvironment;
                    dif.PhonologicalChanges.IsRemove = isRemove;
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
    languageDifference.emplace_back(dif);
    file.close();
    return true;
}
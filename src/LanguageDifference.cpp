#include "..\\include\LanguageDifference.h"
#include "Utility.h"

namespace
{
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
 * @brief Create a Add 単語 object
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param wordForm 語形
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddWord(const std::string &place, const int period, const int wordID, const std::string &wordForm)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::AddWord;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.StringParam_.emplace_back(wordForm);
    return diff;
}

/**
 * @brief Change 言語 影響度
 *
 * @param place 地域
 * @param period 時代
 * @param strength 影響度
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateChangeStrength(const std::string &place, const int period, const double strength)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::ChangeStrength;
    diff.StringParam_.emplace_back(place);
    diff.DoubleParam_.emplace_back(strength);
    return diff;
}

/**
 * @brief Change 言語 音韻
 *
 * @param place 地域
 * @param period 時代
 * @param phonologicalChange 音韻変化
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreatePhonologicalChange(const std::string &place, const int period, const PhonologicalChange phonologicalChange)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::PhonologicalChange;
    diff.StringParam_.emplace_back(place);
    diff.PhonologicalChanges_ = phonologicalChange;
    return diff;
}

/**
 * @brief 借用
 *
 * @param place1 借用元言語地域
 * @param place2 借用先言語地域
 * @param period 時代
 * @param wordID1 借用元単語ID
 * @param wordID2 借用先単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateLoanword(const std::string &place1, const std::string &place2, const int period, const int wordID1, const int wordID2)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::Loanword;
    diff.StringParam_.emplace_back(place1);
    diff.IntParam_.emplace_back(wordID1);
    diff.StringParam_.emplace_back(place2);
    diff.IntParam_.emplace_back(wordID2);
    return diff;
}

/**
 * @brief 複合語
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param wordIDs 参照単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateAddCompound(const std::string &place, const int period, const int wordID, const std::vector<int> wordIDs)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::AddCompound;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.insert(diff.IntParam_.end(), wordIDs.begin(), wordIDs.end());
    return diff;
}

/**
 * @brief 単語削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateObsoleteWord(const std::string &place, const int period, const int wordID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::ObsoleteWord;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    return diff;
}

/**
 * @brief タイプをゲット
 *
 * @return const LanguageDifferenceType
 */
const LanguageDifferenceType &LanguageDifference::GetType() const
{
    return Type_;
}

/**
 * @brief 時代をゲット
 *
 * @return const int
 */
const int LanguageDifference::GetPeriod() const
{
    return Period_;
}

/**
 * @brief 時代を1加算
 *
 */
void LanguageDifference::AddPeriod()
{
    Period_++;
}

/**
 * @brief 時代を1加算
 *
 */
void LanguageDifference::SubPeriod()
{
    Period_--;
}

/**
 * @brief 整数パラメータを取得
 *
 * @param i
 * @return const std::optional<int>
 */
const std::optional<int> LanguageDifference::IntParam(const int i) const
{
    if (i < 0)
    {
        return std::nullopt;
    }
    else if (i >= IntParam_.size())
    {
        return std::nullopt;
    }
    else
    {
        return IntParam_[i];
    }
}

/**
 * @brief 整数パラメータ数
 *
 * @return const int
 */
const int LanguageDifference::IntParamSize() const
{
    return IntParam_.size();
}

/**
 * @brief 実数パラメータを取得
 *
 * @param i
 * @return const std::optional<double>
 */
const std::optional<double> LanguageDifference::DoubleParam(const int i) const
{
    if (i < 0)
    {
        return std::nullopt;
    }
    else if (i >= DoubleParam_.size())
    {
        return std::nullopt;
    }
    else
    {
        return DoubleParam_[i];
    }
}

/**
 * @brief 実数パラメータ数
 *
 * @return const int
 */
const int LanguageDifference::DoubleParamSize() const
{
    return DoubleParam_.size();
}

/**
 * @brief 文字列パラメータを取得
 *
 * @param i
 * @return const std::optional<double>
 */
const std::optional<std::string> LanguageDifference::StringParam(const int i) const
{
    if (i < 0)
    {
        return std::nullopt;
    }
    else if (i >= StringParam_.size())
    {
        return std::nullopt;
    }
    else
    {
        return StringParam_[i];
    }
}

/**
 * @brief 文字列パラメータ数
 *
 * @return const int
 */
const int LanguageDifference::StringParamSize() const
{
    return StringParam_.size();
}

/**
 * @brief 音韻変化を取得
 *
 */
const PhonologicalChange &LanguageDifference::GetPhonologicalChange() const
{
    return PhonologicalChanges_;
}

/**
 * @brief ファイル読み込み
 *
 * @param file
 * @return LanguageDifference
 */
bool LanguageDifference::Import(std::ifstream &file, LanguageDifference &dif)
{
    enum SubMode
    {
        SubMode_Type,
        SubMode_Period,
        SubMode_IntParam,
        SubMode_DoubleParam,
        SubMode_StringParam,
        SubMode_PhonologicalChanges,
    };
    SubMode subMode;

    std::string line;
    while (std::getline(file, line))
    {
        auto [memberName, memberValue] = splitByColon(line);
        if (memberName == "- Section")
        {
            dif = LanguageDifference();
            dif.Period_ = std::stoi(memberValue);
            continue;
        }
        else if (memberName == "Type")
        {
            dif.Type_ = static_cast<LanguageDifferenceType>(std::stoi(memberValue));
            continue;
        }
        else if (line == "    IntParam:")
        {
            subMode = SubMode::SubMode_IntParam;
            continue;
        }
        else if (line == "    DoubleParam:")
        {
            subMode = SubMode::SubMode_DoubleParam;
            continue;
        }
        else if (line == "    StringParam:")
        {
            subMode = SubMode::SubMode_StringParam;
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

            dif.PhonologicalChanges_.BeforePhoneme_ = Phoneme::Create(beforePlace, beforeManner);
            dif.PhonologicalChanges_.AfterPhoneme_ = Phoneme::Create(afterPlace, afterManner);
            dif.PhonologicalChanges_.PhoneticEnvironment_ = phoneticEnvironment;
            dif.PhonologicalChanges_.IsRemove_ = isRemove;

            return true;
        }

        if (subMode == SubMode::SubMode_IntParam)
        {
            dif.IntParam_.emplace_back(std::stoi(line.substr(8)));
        }
        else if (subMode == SubMode::SubMode_DoubleParam)
        {
            dif.DoubleParam_.emplace_back(std::stod(line.substr(8)));
        }
        else if (subMode == SubMode::SubMode_StringParam)
        {
            dif.StringParam_.emplace_back(line.substr(8));
        }
    }
    return false;
}

/**
 * @brief ファイル出力
 *
 * @param file
 */
void LanguageDifference::Export(std::ofstream &file) const
{
    file << "  - Section: " << GetPeriod() << "\n";
    file << "    Type: " << static_cast<int>(GetType()) << "\n";

    file << "    IntParam:\n";
    for (int i = 0; i < IntParamSize(); i++)
    {
        file << "      - " << IntParam(i).value() << "\n";
    }

    file << "    DoubleParam:\n";
    for (int i = 0; i < DoubleParamSize(); i++)
    {
        file << "      - " << DoubleParam(i).value() << "\n";
    }

    file << "    StringParam:\n";
    for (int i = 0; i < StringParamSize(); i++)
    {
        file << "      - " << StringParam(i).value() << "\n";
    }

    file << "    SoundChange:\n";
    file << "      Before:\n";
    file << "        Place: " << GetPhonologicalChange().BeforePhoneme_.GetPlace() << "\n";
    file << "        Mannar: " << GetPhonologicalChange().BeforePhoneme_.GetManner() << "\n";
    file << "      After:\n";
    file << "        Place: " << GetPhonologicalChange().AfterPhoneme_.GetPlace() << "\n";
    file << "        Mannar: " << GetPhonologicalChange().AfterPhoneme_.GetManner() << "\n";
    file << "      Condition: " << static_cast<int>(GetPhonologicalChange().PhoneticEnvironment_) << "\n";
    file << "      IsRemove: " << GetPhonologicalChange().IsRemove_ << "\n";
}
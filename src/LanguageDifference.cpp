#include "LanguageDifference.h"
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
 * @brief 品詞追加
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param partID 品詞ID
 * @param part 品詞名
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateEditPart(const std::string &place, const int period, const int wordID, const int partID, const std::string &part)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::EditPart;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(partID);
    diff.StringParam_.emplace_back(part);
    return diff;
}

/**
 * @brief 品詞削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param partID 品詞ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateDeletePart(const std::string &place, const int period, const int wordID, const int partID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeletePart;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(partID);
    return diff;
}

/**
 * @brief 品詞追加
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param partID 品詞ID
 * @param translationID 訳語ID
 * @param translation 訳語
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateEditTranslation(const std::string &place, const int period, const int wordID, const int partID, const int translationID, const std::string &translation)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::EditTranslation;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(partID);
    diff.IntParam_.emplace_back(translationID);
    diff.StringParam_.emplace_back(translation);
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
    dif = LanguageDifference();
    std::string line;

    // Period
    {
        if (!std::getline(file, line))
            return false;
        const auto period = ParseVector(line);
        if (period.size() < 1)
            return false;
        dif.Period_ = std::stoi(period[0]);
    }
    // Type
    {
        if (!std::getline(file, line))
            return false;
        const auto type = ParseIntVector(line);
        if (type.size() < 1)
            return false;
        dif.Type_ = ConvertToLanguageDifferenceType(type[0]);
    }
    // IntParam
    {
        if (!std::getline(file, line))
            return false;
        const auto param = ParseIntVector(line);
        dif.IntParam_ = param;
    }
    // DoubleParam
    {
        if (!std::getline(file, line))
            return false;
        const auto param = ParseDoubleVector(line);
        dif.DoubleParam_ = param;
    }
    // StringParam
    {
        if (!std::getline(file, line))
            return false;
        const auto param = ParseVector(line);
        dif.StringParam_ = param;
    }
    // PhonologicalChange
    {
        if (!std::getline(file, line))
            return false;
        const auto params = ParseIntVector(line);
        if (params.size() < 6)
        {
            return false;
        }
        dif.PhonologicalChanges_.BeforePhoneme_ = Phoneme::Create(params[0], params[1]);
        dif.PhonologicalChanges_.AfterPhoneme_ = Phoneme::Create(params[2], params[3]);
        dif.PhonologicalChanges_.PhoneticEnvironment_ = ConvertToPhoneticEnvironment(params[4]);
        dif.PhonologicalChanges_.IsRemove_ = static_cast<bool>(params[5]);
    }
    return true;
}

/**
 * @brief ファイル出力
 *
 * @param file
 */
void LanguageDifference::Export(std::ofstream &file) const
{
    file << FormatVector<int>({GetPeriod()}) << "\n";
    file << FormatVector<int>({ConvertFromLanguageDifferenceType(GetType())}) << "\n";
    file << FormatVector<int>(IntParam_) << "\n";
    file << FormatVector<double>(DoubleParam_) << "\n";
    file << FormatVector<std::string>(StringParam_) << "\n";
    std::vector<int> soundChange = {
        GetPhonologicalChange().BeforePhoneme_.GetPlace(),
        GetPhonologicalChange().BeforePhoneme_.GetManner(),
        GetPhonologicalChange().AfterPhoneme_.GetPlace(),
        GetPhonologicalChange().AfterPhoneme_.GetManner(),
        ConvertFromPhoneticEnvironment(GetPhonologicalChange().PhoneticEnvironment_),
        GetPhonologicalChange().IsRemove_};
    file << FormatVector<int>(soundChange) << "\n";
}
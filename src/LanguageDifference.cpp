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
LanguageDifference LanguageDifference::CreateAddWord(const std::string &place, const int period, const int wordID, const std::vector<int> &wordForm)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::AddWord;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.PhonemeIDs_ = wordForm;
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
 * @brief 訳語削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param partID 品詞ID
 * @param translationID 訳語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateDeleteTranslation(const std::string &place, const int period, const int wordID, const int partID, const int translationID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeleteTranslation;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(partID);
    diff.IntParam_.emplace_back(translationID);
    return diff;
}

/**
 * @brief タグ編集
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param tagID タグID
 * @param tag タグ
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateEditTag(const std::string &place, const int period, const int wordID, const int tagID, const std::string &tag)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::EditTag;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(tagID);
    diff.StringParam_.emplace_back(tag);
    return diff;
}

/**
 * @brief タグ削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param tagID タグID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateDeleteTag(const std::string &place, const int period, const int wordID, const int tagID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeleteTag;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(tagID);
    return diff;
}

/**
 * @brief 自由記述編集
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param contentID 自由記述ID
 * @param title タイトル
 * @param content 自由記述
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateEditContent(const std::string &place, const int period, const int wordID, const int contentID, const std::string &title, const std::string &content)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::EditContent;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(contentID);
    diff.StringParam_.emplace_back(title);
    diff.StringParam_.emplace_back(content);
    return diff;
}

/**
 * @brief 自由記述削除
 *
 * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param contentID 自由記述ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateDeleteContent(const std::string &place, const int period, const int wordID, const int contentID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeleteContent;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(contentID);
    return diff;
}

/**
 * @brief 変化形の編集オブジェクトを作成
 * * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param variationID バリエーションID
 * @param title 項目名（変化形のタイトル）
 * @param variation 変化形
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateEditVariation(
    const std::string &place,
    const int period,
    const int wordID,
    const int variationID,
    const std::string &title,
    const std::vector<int> variation)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::EditTranslation; // 列挙型にこの値があることを想定

    // 文字列パラメータ: 0:地域, 1:タイトル
    diff.StringParam_.emplace_back(place);
    diff.StringParam_.emplace_back(title);

    // 数値パラメータ: 0:単語ID, 1:変化形ID
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(variationID);

    // 音素パラメータ
    diff.PhonemeIDs_ = variation;

    return diff;
}

/**
 * @brief 変化形削除オブジェクトを作成
 * * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param variationID 変化形ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateDeleteVariation(const std::string &place, const int period, const int wordID, const int variationID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeleteVariation;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(variationID);
    return diff;
}

/**
 * @brief 関連語の設定オブジェクトを作成
 * * @param place 地域
 * @param period 時代
 * @param wordID 単語ID
 * @param relationID 関連ID
 * @param title 関連タイトル（例: "語源", "派生語"）
 * @param targetWordID 参照先の単語ID
 * @return LanguageDifference
 */
LanguageDifference LanguageDifference::CreateSetRelation(const std::string &place, const int period, const int wordID, const int relationID, const std::string &title, const int targetWordID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::SetRelation;

    // StringParam: [0]=place, [1]=title
    diff.StringParam_.emplace_back(place);
    diff.StringParam_.emplace_back(title);

    // IntParam: [0]=wordID, [1]=relationID, [2]=targetWordID
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(relationID);
    diff.IntParam_.emplace_back(targetWordID);

    return diff;
}

LanguageDifference LanguageDifference::CreateDeleteRelation(const std::string &place, const int period, const int wordID, const int relationID)
{
    LanguageDifference diff;
    diff.Period_ = period;
    diff.Type_ = LanguageDifferenceType::DeleteRelation;
    diff.StringParam_.emplace_back(place);
    diff.IntParam_.emplace_back(wordID);
    diff.IntParam_.emplace_back(relationID);
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
 * @brief 音素列パラメータ
 *
 * @return const std::vector<Phoneme>&
 */
const std::vector<int> &LanguageDifference::GetPhonemeParam() const
{
    return PhonemeIDs_;
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
 * * @param file 入力ストリーム
 * @param dif 読み込み先の差分オブジェクト
 * @return bool 成功したか
 */
bool LanguageDifference::Import(std::ifstream &file, LanguageDifference &dif)
{
    dif = LanguageDifference();
    std::string line;

    // 文字列データの末尾スペースを保護できるように修正
    auto GetCleanLine = [&](std::string &l, bool shouldTrim)
    {
        if (!std::getline(file, l))
            return false;
        // 改行コード \r を除去
        l.erase(std::remove(l.begin(), l.end(), '\r'), l.end());

        if (shouldTrim)
        {
            // 数値解析用：末尾のカンマと空白を除去
            size_t last = l.find_last_not_of(" ,");
            if (last != std::string::npos)
                l = l.substr(0, last + 1);
            else
                l.clear();
        }
        return true;
    };

    int parsedValue = 0;
    int count = 0;

    // 1. Period
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_PERIOD, parsedValue))
        return false;
    dif.Period_ = parsedValue;

    // 2. Type
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_TYPE, parsedValue))
        return false;
    dif.Type_ = ConvertToLanguageDifferenceType(parsedValue);

    // 3. IntParam
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_INT_PARAM, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto val = ParseIntVector(line);
        if (!val.empty())
            dif.IntParam_.push_back(val[0]);
    }

    // 4. DoubleParam
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_DOUBLE_PARAM, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto val = ParseDoubleVector(line);
        if (!val.empty())
            dif.DoubleParam_.push_back(val[0]);
    }

    // 5. StringParam
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_STRING_PARAM, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, false))
            return false;
        auto params = ParseVector(line);
        if (!params.empty())
        {
            std::string s = params[0];
            // エスケープされた "\\n" を実際の "\n" に戻す
            size_t pos = 0;
            while ((pos = s.find("\\n", pos)) != std::string::npos)
            {
                s.replace(pos, 2, "\n");
                pos += 1;
            }
            dif.StringParam_.push_back(s);
        }
    }

    // 6. Form
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_FORM, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto val = ParseIntVector(line);
        if (!val.empty())
            dif.PhonemeIDs_.push_back(val[0]);
    }

    auto &phonologicalChange = dif.PhonologicalChanges_;

    // 7. BeforePhoneme
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_BEFORE_PHONEME, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto vals = ParseIntVector(line);
        if (vals.size() >= 3)
        {
            PhoneticItemType type = static_cast<PhoneticItemType>(vals[0]);
            int id = vals[1];
            FeatureState state = static_cast<FeatureState>(vals[2]);
            phonologicalChange.BeforePhoneticItems_.push_back(PhoneticItem::Create(type, id, state));
        }
    }

    // 8. AfterPhoneme
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_AFTER_PHONEME, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto vals = ParseIntVector(line);
        if (vals.size() >= 3)
        {
            PhoneticItemType type = static_cast<PhoneticItemType>(vals[0]);
            int id = vals[1];
            FeatureState state = static_cast<FeatureState>(vals[2]);
            phonologicalChange.AfterPhoneticItems_.push_back(PhoneticItem::Create(type, id, state));
        }
    }

    // 9. Environment
    if (!GetCleanLine(line, true) || !ParseStringAndInt(line, SECTION_NAME_DIFFERENCES_ENVIRONMENT, count))
        return false;
    for (int i = 0; i < count; ++i)
    {
        if (!GetCleanLine(line, true))
            return false;
        const auto vals = ParseIntVector(line);
        if (vals.size() >= 4)
        {
            int relativePos = vals[0];
            PhoneticItemType type = static_cast<PhoneticItemType>(vals[1]);
            int id = vals[2];
            FeatureState state = static_cast<FeatureState>(vals[3]);
            phonologicalChange.PhoneticEnvironment_.push_back({relativePos, PhoneticItem::Create(type, id, state)});
        }
    }

    return true;
}

/**
 * @brief ファイル出力
 * * @param file 出力ストリーム
 */
void LanguageDifference::Export(std::ofstream &file) const
{
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_PERIOD, GetPeriod()) << "\n";
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_TYPE, ConvertFromLanguageDifferenceType(GetType())) << "\n";

    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_INT_PARAM, IntParam_.size()) << "\n";
    for (const auto &val : IntParam_)
    {
        file << FormatVector<int>({val}) << "\n";
    }

    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_DOUBLE_PARAM, DoubleParam_.size()) << "\n";
    for (const auto &val : DoubleParam_)
    {
        file << FormatVector<double>({val}) << "\n";
    }

    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_STRING_PARAM, StringParam_.size()) << "\n";
    for (auto s : StringParam_)
    {
        size_t pos = 0;
        while ((pos = s.find("\n", pos)) != std::string::npos)
        {
            s.replace(pos, 1, "\\n");
            pos += 2;
        }
        file << FormatVector<std::string>({s}) << "\n";
    }

    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_FORM, PhonemeIDs_.size()) << "\n";
    for (const auto &val : PhonemeIDs_)
    {
        file << FormatVector<int>({val}) << "\n";
    }

    const auto &phonologicalChange = GetPhonologicalChange();

    // 1. BeforePhoneme
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_BEFORE_PHONEME, phonologicalChange.BeforePhoneticItems_.size()) << "\n";
    for (const auto &item : phonologicalChange.BeforePhoneticItems_)
    {
        std::vector<int> row = {static_cast<int>(item.Type_), item.ID_, static_cast<int>(item.State_)};
        file << FormatVector<int>(row) << "\n";
    }

    // 2. AfterPhoneme
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_AFTER_PHONEME, phonologicalChange.AfterPhoneticItems_.size()) << "\n";
    for (const auto &item : phonologicalChange.AfterPhoneticItems_)
    {
        std::vector<int> row = {static_cast<int>(item.Type_), item.ID_, static_cast<int>(item.State_)};
        file << FormatVector<int>(row) << "\n";
    }

    // 3. Environment
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES_ENVIRONMENT, phonologicalChange.PhoneticEnvironment_.size()) << "\n";
    for (const auto &pair : phonologicalChange.PhoneticEnvironment_)
    {
        std::vector<int> row = {pair.first, static_cast<int>(pair.second.Type_), pair.second.ID_, static_cast<int>(pair.second.State_)};
        file << FormatVector<int>(row) << "\n";
    }
}
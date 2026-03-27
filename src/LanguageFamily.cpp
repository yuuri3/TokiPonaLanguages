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
 * * @param geography 地理
 * * @param phonemeTable 音韻
 * @return LanguageFamily
 */
LanguageFamily LanguageFamily::Create(const std::vector<std::vector<std::string>> &geography, const PhonemeTable &phonemeTable)
{
    LanguageFamily languages;
    languages.languageDifference_ = {};
    languages.Geography_.InitializeFromVector(geography);
    languages.PhonemeTable_ = phonemeTable;
    return languages;
}

/**
 * @brief 地理を取得
 * * @return std::vector<std::vector<std::string>>
 */
std::vector<std::vector<std::string>> LanguageFamily::GetGeography() const
{
    return Geography_.ToVector();
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
 * * @param row 行
 * * @param column 列
 * * @param name 地名
 */
void LanguageFamily::ChangePlaceName(const int row, const int column, const std::string &name)
{
    Geography_.SetPlaceName(row, column, name);
}

/**
 * @brief 上行に地理を追加
 * * @param row 行
 */
void LanguageFamily::AddGeomgraphicRowAbove(const int row)
{
    Geography_.AddRowAbove(row);
}

/**
 * @brief 下行に地理を追加
 *
 */
void LanguageFamily::AddGeomgraphicRowBelow(const int row)
{
    Geography_.AddRowBelow(row);
}

/**
 * @brief 地理行を削除
 * * @param row 行
 */
void LanguageFamily::DeleteGeomgraphicRow(const int row)
{
    Geography_.DeleteRow(row);
}

/**
 * @brief 右列に地理を追加
 * * @param column 列
 */
void LanguageFamily::AddGeomgraphicColumnRight(const int column)
{
    Geography_.AddColumnRight(column);
}

/**
 * @brief 左列に地理を追加
 * * @param column 列
 */
void LanguageFamily::AddGeomgraphicColumnLeft(const int column)
{
    Geography_.AddColumnLeft(column);
}

/**
 * @brief 地理の列を削除
 * * @param column 列
 */
void LanguageFamily::DeleteGeomgraphicColumn(const int column)
{
    Geography_.DeleteColumn(column);
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
 * * @return bool 空の場合は true、そうでない場合は false
 */
const bool LanguageFamily::Empty() const
{
    return Geography_.Empty() || PhonemeTable_.Empty();
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
 * @brief 指定された場所と時代の音韻変化を取得
 *
 * @param place 場所
 * @param period 時代
 * @return const std::vector<PhonologicalChange> 音韻変化のリスト
 */
const std::vector<PhonologicalChange> LanguageFamily::GetPhonologicalChanges(const std::string place, const int period)
{
    std::vector<PhonologicalChange> phonologicalChanges;

    for (const auto &languageDifference : languageDifference_)
    {
        if (languageDifference.GetPeriod() == period &&
            languageDifference.GetType() == LanguageDifferenceType::PhonologicalChange)
        {
            const std::optional<std::string> targetPlace = languageDifference.StringParam(0);
            if (targetPlace.has_value() && targetPlace.value() == place)
            {
                phonologicalChanges.emplace_back(languageDifference.GetPhonologicalChange());
            }
        }
    }

    return phonologicalChanges;
}

/**
 * @brief 指定された場所と時代の音韻変化の文字列表現を取得
 * * @param place 場所
 * * @param period 時代
 * * @return const std::vector<std::string>
 */
const std::vector<std::string> LanguageFamily::GetPhonologicalChangeStrings(const std::string place, const int period)
{
    std::vector<std::string> resultStrings;
    const std::vector<PhonologicalChange> changes = GetPhonologicalChanges(place, period);

    for (const auto &change : changes)
    {
        std::string beforeString = PhonemeTable_.ConvertPhoneticItemToString(change.BeforePhoneticItems_);
        std::string afterString = PhonemeTable_.ConvertPhoneticItemToString(change.AfterPhoneticItems_);

        std::string environmentString;
        if (!change.PhoneticEnvironment_.empty())
        {
            std::map<int, std::vector<PhoneticItem>> environmentMap;

            for (const auto &env : change.PhoneticEnvironment_)
            {
                environmentMap[env.first].push_back(env.second);
            }

            std::string leftEnvString;
            std::string rightEnvString;

            for (const auto &pair : environmentMap)
            {
                const int position = pair.first;
                std::string itemString = PhonemeTable_.ConvertPhoneticItemToString(pair.second);

                if (position < 0)
                {
                    if (!leftEnvString.empty())
                    {
                        leftEnvString += " ";
                    }
                    leftEnvString += itemString;
                }
                else if (position > 0)
                {
                    if (!rightEnvString.empty())
                    {
                        rightEnvString += " ";
                    }
                    rightEnvString += itemString;
                }
            }

            environmentString = leftEnvString + DELIMINATOR_ENV_ENV + rightEnvString;
        }

        std::string changeString = beforeString + DELIMINATOR_BEFORE_AFTER + afterString;
        if (!environmentString.empty())
        {
            changeString += DELIMINATOR_AFTER_ENV + environmentString;
        }

        resultStrings.push_back(changeString);
    }

    return resultStrings;
}

/**
 * @brief 指定した場所と時代の音韻変化を設定
 *
 * @param place 場所
 * @param period 時代
 * @param phonologicalChange 音韻変化のリスト
 */
bool LanguageFamily::SetPhonologicalChanges(const std::string place, const int period, const std::vector<PhonologicalChange> &phonologicalChange)
{
    bool result = true;
    languageDifference_.erase(
        std::remove_if(languageDifference_.begin(), languageDifference_.end(),
                       [&](const LanguageDifference &difference)
                       {
                           if (difference.GetPeriod() == period && difference.GetType() == LanguageDifferenceType::PhonologicalChange)
                           {
                               const std::optional<std::string> targetPlace = difference.StringParam(0);
                               if (targetPlace.has_value() && targetPlace.value() == place)
                               {
                                   return true;
                               }
                           }
                           return false;
                       }),
        languageDifference_.end());

    auto insertIterator = std::find_if(languageDifference_.begin(), languageDifference_.end(),
                                       [period](const LanguageDifference &difference)
                                       {
                                           return difference.GetPeriod() > period;
                                       });

    for (const auto &currentChange : phonologicalChange)
    {
        if (!currentChange.IsValid())
        {
            result = false;
            continue;
        }
        LanguageDifference newDifference = LanguageDifference::CreatePhonologicalChange(place, period, currentChange);
        insertIterator = languageDifference_.insert(insertIterator, newDifference);
        ++insertIterator;
    }
    return result;
}

/**
 * @brief 文字列のリストから音韻変化を設定
 *
 * @param place 場所
 * @param period 時代
 * @param phonologicalChange 音韻変化の文字列リスト
 */
bool LanguageFamily::SetPhonologicalChangesFromString(const std::string place, const int period, const std::vector<std::string> &phonologicalChange)
{
    std::vector<PhonologicalChange> parsedChanges;

    auto trimString = [](std::string str)
    {
        size_t first = str.find_first_not_of(" \t");
        if (std::string::npos == first)
            return std::string();
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, (last - first + 1));
    };

    for (const auto &changeString : phonologicalChange)
    {
        PhonologicalChange newChange;

        std::string ruleString = changeString;
        std::string envString = "";

        // " / " で分割して環境部分を抽出
        size_t slashPos = changeString.find(DELIMINATOR_AFTER_ENV);
        if (slashPos != std::string::npos)
        {
            ruleString = changeString.substr(0, slashPos);
            envString = changeString.substr(slashPos + DELIMINATOR_AFTER_ENV.length());
        }

        std::string beforeString = "";
        std::string afterString = "";
        // " > " で分割して変化前後を抽出
        size_t arrowPos = ruleString.find(DELIMINATOR_BEFORE_AFTER);
        if (arrowPos != std::string::npos)
        {
            beforeString = trimString(ruleString.substr(0, arrowPos));
            afterString = trimString(ruleString.substr(arrowPos + DELIMINATOR_BEFORE_AFTER.length()));
        }
        else
        {
            beforeString = trimString(ruleString);
        }

        // 変化前の解析
        newChange.BeforePhoneticItems_ = PhonemeTable_.ConvertStringToPhoneticItem(beforeString);

        // 変化後の解析
        newChange.AfterPhoneticItems_ = PhonemeTable_.ConvertStringToPhoneticItem(afterString);

        // 環境（u _ i など）の解析
        if (!envString.empty())
        {
            // 定数 " _ " をそのまま使用して左右に分割
            size_t envPos = envString.find(DELIMINATOR_ENV_ENV);
            if (envPos != std::string::npos)
            {
                std::string leftEnv = envString.substr(0, envPos);
                std::string rightEnv = envString.substr(envPos + DELIMINATOR_ENV_ENV.length());

                // 左側のトークン群（位置：-n, ..., -1）
                const std::vector<PhoneticItem> leftItems = PhonemeTable_.ConvertStringToPhoneticItem(leftEnv);
                int index = -leftItems.size();
                for (const auto leftItem : leftItems)
                {
                    newChange.PhoneticEnvironment_.push_back({index, leftItem});
                    index++;
                }
                // 右側のトークン群（位置：1, 2, ...）
                const std::vector<PhoneticItem> rightItems = PhonemeTable_.ConvertStringToPhoneticItem(rightEnv);
                index = 1;
                for (const auto rightItem : rightItems)
                {
                    newChange.PhoneticEnvironment_.push_back({index, rightItem});
                    index++;
                }
            }
        }

        parsedChanges.push_back(newChange);
    }

    return SetPhonologicalChanges(place, period, parsedChanges);
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
 * * @param filename 出力ファイル名
 */
void LanguageFamily::Export(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return;
    }

    // 2. Map (地理情報)
    file << JoinStringAndInt(SECTION_NAME_GEOGRAPHY, 0) << "\n";
    Geography_.Export(file);

    // 3. PhonemeTable (音韻マスタデータ)
    // 旧 PhoneticsMap セクションは、詳細なマスタデータ構造へ置き換え
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE, 0) << "\n";
    PhonemeTable_.Export(file);

    // 4. LanguageDifferences (言語変化の記録)
    file << JoinStringAndInt(SECTION_NAME_DIFFERENCES, languageDifference_.size()) << "\n";
    for (const auto &diff : languageDifference_)
    {
        diff.Export(file);
    }

    file.close();
}

/**
 * @brief 差分（および音韻マスタ・地図）をファイル読み込み
 * * @param filename 入力ファイル名
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
            int count = 0;
            if (ParseStringAndInt(line, SECTION_NAME_GEOGRAPHY, count))
            {
                if (!Geography_.Import(file))
                {
                    return false;
                }
            }
            else if (ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE, count))
            {
                if (!PhonemeTable_.Import(file))
                {
                    return false;
                }
            }
            else if (ParseStringAndInt(line, SECTION_NAME_DIFFERENCES, count))
            {
                languageDifference_.clear();
                languageDifference_.reserve(count); // 速度改善：メモリ予約

                for (int i = 0; i < count; ++i)
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
 * * @param filename ファイル名
 * @return bool 成功したか
 */
bool LanguageFamily::ImportJson(const std::string &filename)
{
    // 地理データ初期化
    Geography_.InitializeFromVector({{"0"}});

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
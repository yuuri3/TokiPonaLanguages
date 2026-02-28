#include "LanguageFamilySimulator.h"
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace
{

    // ヘルパー：vectorの中身を文字列に変換
    template <typename T>
    std::string joinVector(const std::vector<T> &vec, const std::string &delimiter = " ")
    {
        std::stringstream ss;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            ss << vec[i] << (i == vec.size() - 1 ? "" : delimiter);
        }
        return ss.str();
    }

    // ヘルパー関数：列挙型を文字列に変換（可読性向上）
    std::string getDiffTypeName(LanguageDifferenceType type)
    {
        switch (type)
        {
        case LanguageDifferenceType::ChangeStrength:
            return "ChangeStrength";
        case LanguageDifferenceType::PhonologicalChange:
            return "ChangeSound";
        case LanguageDifferenceType::Loanword:
            return "BorrowWord";
        case LanguageDifferenceType::AddCompound:
            return "AddCompoundWord";
        case LanguageDifferenceType::ObsoleteWord:
            return "Remove";
        default:
            return "Unknown";
        }
    }

    /**
     * @brief 祖語をセット
     *
     * @param geometryData 地理データ
     * @param startPlace 祖語を配置する地域
     * @param protoLanguage 祖語
     * @return std::map<std::string, Language>
     */
    std::map<std::string, Language> setProtoLanguageOnMap(
        const std::vector<std::string> &geometryData,
        const std::string &startPlace,
        const Language &protoLanguage)
    {
        std::map<std::string, Language> languageOnGeometry;

        for (const std::string &place : geometryData)
        {
            // 1番目の要素をitem、2番目を空文字で初期化してペアを作成
            Language language;
            language.Reset();

            // もし1番目の要素がtargetAと一致したら、2番目をreplacementBにする
            if (place == startPlace)
            {
                language = protoLanguage;
            }

            // 位置を設定
            languageOnGeometry[place] = language;
        }

        return languageOnGeometry;
    }

    /**
     * 変化規則をランダムに生成
     * @param beforePlace 変化前音素
     * @param beforeMannar 変化前音素
     * @param table 音素表
     * @param pRemovePhoneme 音が脱落する確率
     */
    PhonologicalChange makepPhonologicalChangeRandom(const Phoneme &beforePhon, const std::vector<std::vector<std::string>> &phonemeTable, const double pRemoveSound)
    {
        int randomPhoneticEnvironment = getRandomInt(0, 2);

        PhonologicalChange randomPhonologicalChange;
        randomPhonologicalChange.BeforePhoneme_ = beforePhon;
        randomPhonologicalChange.IsRemove_ = getWithProbability(pRemoveSound);
        switch (randomPhoneticEnvironment)
        {
        case 0:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::Start;
            break;
        case 1:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::Middle;
            break;
        case 2:
            randomPhonologicalChange.PhoneticEnvironment_ = PhoneticEnvironment::End;
            break;

        default:
            break;
        }
        randomPhonologicalChange.AfterPhoneme_ = beforePhon;
        moveRandomOnTable(randomPhonologicalChange.AfterPhoneme_.Manner_, randomPhonologicalChange.AfterPhoneme_.Place_, phonemeTable);
        return randomPhonologicalChange;
    }

    /**
     * @brief 音素表から、音素をランダムに1つ選択する
     * @param phonemeTable 音素表
     * @return 音素
     */
    Phoneme getRandomSoundFromTable(const std::vector<std::vector<std::string>> &phonemeTable)
    {
        const auto converter = PhonemeConverter::Create(phonemeTable);
        return converter.GetRandom();
    }
}

/**
 * 地図データの特定の位置に祖語を配置する
 * @param startPlace 祖語を配置する位置
 * @param protoLanguage 祖語
 */
void LanguageFamilySimulator::SetProtoLanguageOnGeography(
    const std::string &startPlace,
    const Language &protoLanguage)
{
    Languages_ = setProtoLanguageOnMap(getNonEmptyStrings(LanguageFamily_.GetGeography()), startPlace, protoLanguage);
    ProtoLanguage_ = protoLanguage;

    // ログ
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);
    LanguageFamily_.languageDifference_.emplace_back(LanguageDifference::CreateChangeStrength(startPlace, Period_, protoLanguage.GetStrength()));

    for (int i = 0; i < protoLanguage.CountWord(); i++)
    {
        const auto [wordID, word] = protoLanguage.GetNthWord(i);

        LanguageFamily_.languageDifference_.emplace_back(LanguageDifference::CreateAddWord(startPlace, Period_, wordID, converter.ConvertToString(word.GetForm())));
    }
}

/**
 * @brief Get the 単語 object
 *
 * @param place 位置
 * @return std::vector<std::string>
 */
std::vector<std::string> LanguageFamilySimulator::GetWords(std::string place)
{
    if (Languages_.count(place) == 0)
    {
        return {};
    }
    const auto language = Languages_[place];
    std::vector<std::string> words;
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);

    for (int i = 0; i < language.CountWord(); i++)
    {
        const auto [_, word] = language.GetNthWord(i);

        words.emplace_back(converter.ConvertToString(word.GetForm()));
    }
    return words;
}

/**
 * 音変化
 * @param pPhonologicalChange 音韻変化確率
 * @param pSoundLoss 音素脱落確率
 * @param isProhibitMinimalPair ミニマルペアを禁止するか
 * @param isSoundDuplication 音素の重複を禁止するか
 *
 * @note ある言語の単語を一斉に変化させる。
 */
void LanguageFamilySimulator::PhonologicalChangeRandom(
    const double pPhonologicalChange,
    const double pSoundLoss,
    const bool isProhibitMinimalPair,
    const bool isProhibitSoundDuplication)
{
    for (auto &[place, language] : Languages_)
    {
        // 音韻変化するかどうか
        if (!getWithProbability(pPhonologicalChange))
        {
            continue;
        }
        // 言語があるか
        if (language.Empty())
        {
            continue;
        }
        const auto randomSound = getRandomSoundFromTable(LanguageFamily_.PhonemeTable_);
        PhonologicalChange randomPhonologicalChange = makepPhonologicalChangeRandom(randomSound, LanguageFamily_.PhonemeTable_, pSoundLoss);

        // ログ
        const auto dif = LanguageDifference::CreatePhonologicalChange(place, Period_, randomPhonologicalChange);
        LanguageFamily_.languageDifference_.emplace_back(dif);

        language.ApplyDifference(dif);
    }
}

/**
 * 単語を借用
 *
 * @param nLoanword 借用回数
 * @param pLoanword 借用率
 *
 * @note 借用の履歴をlanguageに記録
 */
void LanguageFamilySimulator::LoanwordRandom(const int nLoanword, const double pLoanword)
{
    const auto geographyAdjacentData = getAdjacencies(LanguageFamily_.GetGeography());
    for (int i = 0; i < nLoanword; i++)
    {
        // 借用率 は現在固定
        const auto adjucent = geographyAdjacentData[getRandomInt(0, geographyAdjacentData.size() - 1)];
        {
            auto languageIterator1 = Languages_.find(adjucent.first);
            auto languageIterator2 = Languages_.find(adjucent.second);
            if (languageIterator1 == Languages_.end() || languageIterator2 == Languages_.end())
                continue;

            Language &language1 = languageIterator1->second;
            Language &language2 = languageIterator2->second;

            if (language1.Empty() || language2.Empty())
            {
                if (language1.Empty())
                {
                    language1.Copy(language2);

                    // ログ
                    for (int i = 0; i < language2.CountWord(); i++)
                    {
                        const auto [wordID, word] = language2.GetNthWord(i);

                        const auto dif = LanguageDifference::CreateLoanword(languageIterator2->first, languageIterator1->first, Period_, wordID, wordID);
                        LanguageFamily_.languageDifference_.emplace_back(dif);
                    }
                }
                else
                {
                    language2.Copy(language1);

                    // ログ
                    for (int i = 0; i < language1.CountWord(); i++)
                    {
                        const auto [wordID, word] = language1.GetNthWord(i);

                        const auto dif = LanguageDifference::CreateLoanword(languageIterator1->first, languageIterator2->first, Period_, wordID, wordID);
                        LanguageFamily_.languageDifference_.emplace_back(dif);
                    }
                }
                continue;
            }

            auto *referenceLanguage = (language1.IsStronger(language2)) ? &language1 : &language2;
            auto *targetLanguage = (language1.IsStronger(language2)) ? &language2 : &language1;
            const auto referencePlace = (language1.IsStronger(language2)) ? languageIterator1->first : languageIterator2->first;
            const auto targetPlace = (language1.IsStronger(language2)) ? languageIterator2->first : languageIterator1->first;

            for (int i = 0; i < targetLanguage->CountWord(); i++)
            {
                const auto [targetWordID, _] = targetLanguage->GetNthWord(i);

                // 借用率は 0.5 で固定
                if (getRandomInt(0, 1) != 0)
                    continue;

                // ログ
                const auto dif = LanguageDifference::CreateLoanword(referencePlace, targetPlace, Period_, targetWordID, targetWordID);
                LanguageFamily_.languageDifference_.emplace_back(dif);

                targetLanguage->LoanWord(dif, *referenceLanguage);
            }
        }
    }
}

/**
 * @brief 言語の影響度をランダムに変化させる
 *
 * @param pChangeStrength 変化率
 */
void LanguageFamilySimulator::ChangeLanguageStrengthRandom(const double pChangeStrength)
{
    for (auto &[place, language] : Languages_)
    {
        if (getWithProbability(pChangeStrength))
        {
            // ログ
            const auto dif = language.ChangeStrength(place, Period_);
            LanguageFamily_.languageDifference_.emplace_back(dif);
        }
    }
}

/**
 * @brief 各地に言語があるか
 *
 * @return true
 * @return false
 */
bool LanguageFamilySimulator::HasAllPlaceLanguage()
{
    const auto places = getNonEmptyStrings(LanguageFamily_.GetGeography());
    for (const auto &place : places)
    {
        // find を使うことで「存在チェック」と「データアクセス」を1回で済ませる
        auto it = Languages_.find(place);
        if (it == Languages_.end() || it->second.Empty())
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 時代を進める
 *
 */
void LanguageFamilySimulator::ToNextPeriod()
{
    Period_++;
}

/**
 * @brief 差分を適用
 *
 * @param diff 差分
 *
 * @return 成否
 */
bool LanguageFamilySimulator::ApplyDifference(const LanguageDifference &diff)
{
    const auto places = getNonEmptyStrings(LanguageFamily_.GetGeography());
    PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);

    switch (diff.GetType())
    {
    case LanguageDifferenceType::AddWord:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        const auto wordID = diff.IntParam(0);
        if (!wordID)
        {
            return false;
        }
        const auto form = diff.StringParam(1);
        if (!form)
        {
            return false;
        }

        Languages_[*geometry].AddWord(diff, converter.ConvertToPhoneme(*form));
        if (diff.GetPeriod() == 0 && geometry == "0")
        {
            ProtoLanguage_.AddWord(diff, converter.ConvertToPhoneme(*form));
        }
        break;
    }

    case LanguageDifferenceType::ChangeStrength:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }

        if (Languages_.count(*geometry) == 1)
        {
            Languages_[*geometry].ApplyDifference(diff);
        }
        break;
    }

    case LanguageDifferenceType::PhonologicalChange:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }

        if (Languages_.count(*geometry) == 1)
        {
            Languages_[*geometry].ApplyDifference(diff);
        }
        break;
    }

    case LanguageDifferenceType::Loanword:
    {
        const auto referenceGeometry = diff.StringParam(0);
        if (!referenceGeometry)
        {
            return false;
        }
        const auto targetGeometry = diff.StringParam(1);
        if (!targetGeometry)
        {
            return false;
        }
        const auto referenceWordID = diff.IntParam(0);
        if (!referenceWordID)
        {
            return false;
        }
        const auto targetWordID = diff.IntParam(1);
        if (!targetWordID)
        {
            return false;
        }

        if (Languages_.count(*referenceGeometry) == 1)
        {
            const auto referenceLanguage = Languages_.at(*referenceGeometry);
            Languages_[*targetGeometry].LoanWord(diff, referenceLanguage);
        }
        break;
    }

    case LanguageDifferenceType::AddCompound:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        Languages_[*geometry].ApplyDifference(diff);
        break;
    }

    case LanguageDifferenceType::ObsoleteWord:
    {
        const auto geometry = diff.StringParam(0);
        if (!geometry)
        {
            return false;
        }
        Languages_[*geometry].ApplyDifference(diff);
        break;
    }

    default:
    {
        return false;
    }
    }
    return true;
}

/**
 * @brief 差分を複数適用
 *
 * @param diffs 差分
 *
 * @return 成否
 */
bool LanguageFamilySimulator::ApplyDifferences(const std::vector<LanguageDifference> &diffs)
{
    for (const auto &diff : diffs)
    {
        if (!ApplyDifference(diff))
        {
            return false;
        }
    }
    return true;
}

std::optional<Language> LanguageFamilySimulator::CalculateLanguage(const std::string place, const int period)
{
    for (const auto &diff : LanguageFamily_.languageDifference_)
    {
        if (diff.GetPeriod() > period)
        {
            return Languages_.at(place);
        }
        if (!ApplyDifference(diff))
        {
            return std::nullopt;
        }
    }
    return Languages_.at(place);
}

/**
 * @brief インスタンス生成
 *
 */
std::optional<LanguageFamilySimulator> LanguageFamilySimulator::Create()
{
    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_ = LanguageFamily::Create({{""}}, {{""}});
    simulator.Period_ = 0;
    simulator.Languages_.clear();
    simulator.ProtoLanguage_ = Language();

    return simulator;
}

/**
 * @brief インスタンス生成
 *
 * @param languageFamily 語族
 */
std::optional<LanguageFamilySimulator> LanguageFamilySimulator::Create(LanguageFamily languageFamily)
{
    LanguageFamilySimulator simulator;
    simulator.LanguageFamily_ = languageFamily;
    simulator.Period_ = 0;
    simulator.Languages_.clear();
    simulator.ProtoLanguage_ = Language();

    if (!simulator.ApplyDifferences(languageFamily.languageDifference_))
    {
        return std::nullopt;
    }
    return simulator;
}

/**
 * @brief 言語名の配列を出力
 *
 * @return std::vector<std::vector<std::string>>
 */
std::vector<std::vector<std::string>> LanguageFamilySimulator::ToStringLanguageFamily()
{
    int currentPeriod = 0;
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;
    Period_ = 0;
    Languages_.clear();

    auto converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.GetGeography()))
    {
        line.emplace_back(place);
    }
    result.emplace_back(line);
    line.clear();

    for (const auto &diff : LanguageFamily_.languageDifference_)
    {
        if (diff.GetPeriod() != currentPeriod)
        {
            currentPeriod = diff.GetPeriod();
            for (const auto &place : getNonEmptyStrings(LanguageFamily_.GetGeography()))
            {
                if (Languages_.count(place) == 0 || Languages_[place].Empty())
                {
                    line.emplace_back("");
                }
                else
                {
                    line.emplace_back(converter.ConvertToString(Languages_[place].GetWord(0)->GetForm()));
                }
            }
            result.emplace_back(line);
            line.clear();
        }
        if (!ApplyDifference(diff))
        {
            return {};
        }
    }

    for (const auto &place : getNonEmptyStrings(LanguageFamily_.GetGeography()))
    {
        if (Languages_.count(place) == 0 || Languages_[place].Empty())
        {
            line.emplace_back("");
        }
        else
        {
            line.emplace_back(converter.ConvertToString(Languages_[place].GetWord(0)->GetForm()));
        }
    }
    result.emplace_back(line);
    line.clear();

    return result;
}

/**
 * @brief 文字列の配列に変換
 *
 * @return std::vector<std::vector<std::string>> 配列
 */
std::vector<std::vector<std::string>> LanguageFamilySimulator::ToStringCurrentLanguages()
{
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> line;

    // 1. 文字列変換の結果をキャッシュするマップ (高速化の肝)
    std::map<std::vector<Phoneme>, std::string> convertCache;
    auto getCachedString = [&](const std::vector<Phoneme> &s) -> const std::string &
    {
        auto iterator = convertCache.find(s);
        if (iterator != convertCache.end())
            return iterator->second;
        PhonemeConverter converter = PhonemeConverter::Create(LanguageFamily_.PhonemeTable_);
        return convertCache[s] = converter.ConvertToString(s); //
    };

    // 2. ヘッダー行 (Place) の出力と、Languageポインタのキャッシュ
    line.emplace_back("");
    std::vector<const Language *> languagePtrList;
    languagePtrList.reserve(Languages_.size());
    for (const auto &[place, language] : Languages_)
    {
        line.emplace_back(place);
        languagePtrList.push_back(&language); // ループ内でのmap検索を避けるために保持
    }
    result.emplace_back(line);
    line.clear();

    // 5. 各単語の出力
    for (int i = 0; i < ProtoLanguage_.CountWord(); i++)
    {
        const auto [wordID, word] = ProtoLanguage_.GetNthWord(i);

        line.emplace_back(getCachedString(word.GetForm()));
        for (const auto &[__, language] : Languages_)
        {
            line.emplace_back(getCachedString(language.GetWord(wordID)->GetForm()));
        }
        result.emplace_back(line);
        line.clear();
    }
    return result;
}
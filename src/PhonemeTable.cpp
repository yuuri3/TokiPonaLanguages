#include "PhonemeTable.h"
#include "Utility.h"

/**
 * @brief 音素IDの配列を音素名称を連結した文字列に変換する
 * @param form 音素IDの配列
 * @return 連結された文字列
 */
std::string PhonemeTable::ConvertToString(const std::vector<int> &form) const
{
    std::string resultString = "";

    for (int phonemeId : form)
    {
        auto it = Phonemes_.find(phonemeId);
        if (it != Phonemes_.end())
        {
            resultString += it->second;
        }
    }

    return resultString;
}

/**
 * @brief 音素名称の文字列を解析し、音素IDの配列に変換する
 * * @param phonemeIDs 連結された音素名称の文字列
 * @return 変換後の音素IDの配列
 */
std::vector<int> PhonemeTable::ConvertToPhoneme(const std::string &phonemeIDs) const
{
    std::vector<int> resultIds;
    size_t offset = 0;

    while (offset < phonemeIDs.length())
    {
        int bestMatchId = -1;
        size_t maxLength = 0;

        // 登録されている全音素から最長一致する名称を探索
        for (const auto &[id, name] : Phonemes_)
        {
            if (!name.empty() && phonemeIDs.compare(offset, name.length(), name) == 0)
            {
                if (name.length() > maxLength)
                {
                    maxLength = name.length();
                    bestMatchId = id;
                }
            }
        }

        if (bestMatchId != -1)
        {
            resultIds.push_back(bestMatchId);
            offset += maxLength;
        }
        else
        {
            // どの音素名称とも一致しない文字はスキップ
            offset++;
        }
    }

    return resultIds;
}

/**
 * @brief 音素が1つ以上定義されているかどうかを判定する
 * * @return bool 音素が定義されていなければ true、定義されていれば false
 */
bool PhonemeTable::Empty() const
{
    return Phonemes_.empty();
}

/**
 * @brief 音素が指定された音韻条件に合致するか判定する
 * * @param phonemeId 判定対象の音素ID
 * * @param condition 判定条件（音韻要素）
 * @return bool 条件に合致すればtrue
 */
bool PhonemeTable::Matches(const int phonemeId, const PhoneticItem &condition) const
{
    switch (condition.Type_)
    {
    case PhoneticItemType::Phoneme:
        return phonemeId == condition.ID_;

    case PhoneticItemType::Place:
    case PhoneticItemType::Manner:
    {
        for (const auto &[placeMannerPair, mappedPhonemeId] : MapPlaceMannerToPhoneme_)
        {
            if (mappedPhonemeId == phonemeId)
            {
                if (condition.Type_ == PhoneticItemType::Place)
                {
                    return placeMannerPair.first == condition.ID_;
                }
                else
                {
                    return placeMannerPair.second == condition.ID_;
                }
            }
        }
        return false;
    }

    case PhoneticItemType::Feature:
    {
        auto phonemeIt = PhonemeFeatureRelations_.find(phonemeId);
        if (phonemeIt != PhonemeFeatureRelations_.end())
        {
            auto featureIt = phonemeIt->second.find(condition.ID_);
            if (featureIt != phonemeIt->second.end())
            {
                return featureIt->second == condition.State_;
            }
        }
        return false;
    }

    default:
        return false;
    }
}

/**
 * @brief 音韻マスタデータ（素性・部位・方法・音素・関連性）を出力
 *
 * @param file 出力ストリーム
 */
void PhonemeTable::Export(std::ofstream &file) const
{
    // 1. Features (ID, Name)
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_FEATURE, static_cast<int>(Features_.size())) << "\n";
    for (const auto &[id, name] : Features_)
    {
        file << id << "," << name << "\n";
    }

    // 2. Places (Order, ID, Name)
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_PLACE, static_cast<int>(PlaceOrder_.size())) << "\n";
    for (int id : PlaceOrder_)
    {
        file << id << "," << Places_.at(id) << "\n";
    }

    // 3. Manners (Order, ID, Name)
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_MANNER, static_cast<int>(MannerOrder_.size())) << "\n";
    for (int id : MannerOrder_)
    {
        file << id << "," << Manner_.at(id) << "\n";
    }

    // 4. Phonemes (ID, Name)
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_PHONEME, static_cast<int>(Phonemes_.size())) << "\n";
    for (const auto &[id, name] : Phonemes_)
    {
        file << id << "," << name << "\n";
    }

    // 5. Relations (Place-Feature, Manner-Feature, Phoneme-Feature)
    // 構造: 対象ID, 素性ID, 状態(int) の組をシリアライズ
    auto ExportRelations = [&](const std::string &label, const auto &relMap)
    {
        int elementCount = 0;
        for (const auto &[targetId, featureMap] : relMap)
        {
            elementCount += static_cast<int>(featureMap.size());
        }

        file << JoinStringAndInt(label, elementCount) << "\n";
        for (const auto &[targetId, featureMap] : relMap)
        {
            for (const auto &[fId, state] : featureMap)
            {
                std::vector<int> row = {targetId, fId, static_cast<int>(state)};
                file << FormatVector<int>(row) << "\n";
            }
        }
    };

    ExportRelations(SECTION_NAME_PHONEMETABLE_PLACE_FEATURE, PlaceFeatureRelations_);
    ExportRelations(SECTION_NAME_PHONEMETABLE_MANNER_FEATURE, MannerFeatureRelations_);

    // PhonemeFeature は std::map<int, FeatureState> なので個別に処理
    int pElementCount = 0;
    for (const auto &[pId, fMap] : PhonemeFeatureRelations_)
    {
        pElementCount += static_cast<int>(fMap.size());
    }

    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_PHONEME_FEATURE, pElementCount) << "\n";
    for (const auto &[pId, fMap] : PhonemeFeatureRelations_)
    {
        for (const auto &[fId, state] : fMap)
        {
            std::vector<int> row = {pId, fId, static_cast<int>(state)};
            file << FormatVector<int>(row) << "\n";
        }
    }

    // 6. Grid Map (PlaceID, MannerID -> PhonemeID)
    file << JoinStringAndInt(SECTION_NAME_PHONEMETABLE_GRIDMAP, static_cast<int>(MapPlaceMannerToPhoneme_.size())) << "\n";
    for (const auto &[pair, pId] : MapPlaceMannerToPhoneme_)
    {
        std::vector<int> row = {pair.first, pair.second, pId};
        file << FormatVector<int>(row) << "\n";
    }
}

/**
 * @brief 音韻マスタデータを読み込み
 *
 * @param file 入力ストリーム
 * @return bool 成功したか
 */
bool PhonemeTable::Import(std::ifstream &file)
{
    std::string line;
    auto GetNextLine = [&]()
    {
        if (std::getline(file, line))
            return true;
        else
            return false;
    };

    int count = 0;
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_FEATURE, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseVector(line);
            Features_[std::stoi(parts[0])] = parts[1];
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_PLACE, count))
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseVector(line);
            PlaceOrder_.emplace_back(std::stoi(parts[0]));
            Places_[std::stoi(parts[0])] = parts[1];
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_MANNER, count))
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseVector(line);
            MannerOrder_.emplace_back(std::stoi(parts[0]));
            Manner_[std::stoi(parts[0])] = parts[1];
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_PHONEME, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseVector(line);
            Phonemes_[std::stoi(parts[0])] = parts[1];
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_PLACE_FEATURE, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseIntVector(line);
            PlaceFeatureRelations_[parts[0]][parts[1]] = static_cast<FeatureState>(parts[2]);
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_MANNER_FEATURE, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseIntVector(line);
            MannerFeatureRelations_[parts[0]][parts[1]] = static_cast<FeatureState>(parts[2]);
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_PHONEME_FEATURE, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseIntVector(line);
            PhonemeFeatureRelations_[parts[0]][parts[1]] = static_cast<FeatureState>(parts[2]);
        }
    }
    else
    {
        return false;
    }
    if (GetNextLine() && ParseStringAndInt(line, SECTION_NAME_PHONEMETABLE_GRIDMAP, count))
    {
        for (int i = 0; i < count; ++i)
        {
            if (!GetNextLine())
                return false;
            auto parts = ParseIntVector(line);
            MapPlaceMannerToPhoneme_[{parts[0], parts[1]}] = parts[2];
        }
    }
    else
    {
        return false;
    }
    return true;
}

/**
 * @brief アルファベット一式を音素として持つ仮の音素表を生成する
 * * @return PhonemeTable 生成された仮の音素表
 */
PhonemeTable PhonemeTable::CreateDummyTable()
{
    PhonemeTable dummyTable;

    for (int i = 0; i < 26; ++i)
    {
        char alphabet = 'a' + i;
        std::string phonemeName(1, alphabet);

        // 音素ID i に対してアルファベット 1 文字を割り当て
        dummyTable.Phonemes_[i] = phonemeName;
    }
    dummyTable.Phonemes_[27] = " ";

    return dummyTable;
}
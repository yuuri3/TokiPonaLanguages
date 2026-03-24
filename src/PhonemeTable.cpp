#include "PhonemeTable.h"
#include "Utility.h"

/**
 * @brief std::pair<int, int> のハッシュ値を計算する
 * * @param p ハッシュ化する整数のペア
 * @return std::size_t 計算されたハッシュ値
 */
std::size_t PairHash::operator()(const std::pair<int, int> &p) const
{
    auto hash1 = std::hash<int>{}(p.first);
    auto hash2 = std::hash<int>{}(p.second);
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}

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
    file << "Features:\n";
    file << static_cast<int>(Features_.size()) << "\n";
    for (const auto &[id, name] : Features_)
    {
        file << id << "," << name << "\n";
    }

    // 2. Places (Order, ID, Name)
    file << "Places:\n";
    file << FormatVector<int>(PlaceOrder_) << "\n";
    for (int id : PlaceOrder_)
    {
        file << id << "," << Places_.at(id) << "\n";
    }

    // 3. Manners (Order, ID, Name)
    file << "Manners:\n";
    file << FormatVector<int>(MannerOrder_) << "\n";
    for (int id : MannerOrder_)
    {
        file << id << "," << Manner_.at(id) << "\n";
    }

    // 4. Phonemes (ID, Name)
    file << "Phonemes:\n";
    file << static_cast<int>(Phonemes_.size()) << "\n";
    for (const auto &[id, name] : Phonemes_)
    {
        file << id << "," << name << "\n";
    }

    // 5. Relations (Place-Feature, Manner-Feature, Phoneme-Feature)
    // 構造: 対象ID, 素性ID, 状態(int) の組をシリアライズ
    auto ExportRelations = [&](const std::string &label, const auto &relMap)
    {
        file << label << ":\n";
        std::vector<int> flattened;
        for (const auto &[targetId, featureMap] : relMap)
        {
            for (const auto &[fId, state] : featureMap)
            {
                flattened.push_back(targetId);
                flattened.push_back(fId);
                flattened.push_back(static_cast<int>(state));
            }
        }
        file << static_cast<int>(flattened.size() / 3) << "\n";
        file << FormatVector<int>(flattened) << "\n";
    };

    ExportRelations("PlaceFeature", PlaceFeatureRelations_);
    ExportRelations("MannerFeature", MannerFeatureRelations_);

    // PhonemeFeature は std::map<int, FeatureState> なので個別に処理
    file << "PhonemeFeature:\n";
    std::vector<int> pFlattened;
    for (const auto &[pId, fMap] : PhonemeFeatureRelations_)
    {
        for (const auto &[fId, state] : fMap)
        {
            pFlattened.push_back(pId);
            pFlattened.push_back(fId);
            pFlattened.push_back(static_cast<int>(state));
        }
    }
    file << static_cast<int>(pFlattened.size() / 3) << "\n";
    file << FormatVector<int>(pFlattened) << "\n";

    // 6. Grid Map (PlaceID, MannerID -> PhonemeID)
    file << "GridMap:\n";
    std::vector<int> gFlattened;
    for (const auto &[pair, pId] : MapPlaceMannerToPhoneme_)
    {
        gFlattened.push_back(pair.first);  // Place
        gFlattened.push_back(pair.second); // Manner
        gFlattened.push_back(pId);
    }
    file << static_cast<int>(gFlattened.size() / 3) << "\n";
    file << FormatVector<int>(gFlattened) << "\n";
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

    while (GetNextLine())
    {
        if (line == "Features:")
        {
            if (!GetNextLine())
                return false;
            int count = std::stoi(line);
            for (int i = 0; i < count; ++i)
            {
                if (!GetNextLine())
                    return false;
                auto parts = ParseVector(line);
                Features_[std::stoi(parts[0])] = parts[1];
            }
        }
        else if (line == "Places:")
        {
            if (!GetNextLine())
                return false;
            PlaceOrder_ = ParseIntVector(line);
            for (size_t i = 0; i < PlaceOrder_.size(); ++i)
            {
                if (!GetNextLine())
                    return false;
                auto parts = ParseVector(line);
                Places_[std::stoi(parts[0])] = parts[1];
            }
        }
        else if (line == "Manners:")
        {
            if (!GetNextLine())
                return false;
            MannerOrder_ = ParseIntVector(line);
            for (size_t i = 0; i < MannerOrder_.size(); ++i)
            {
                if (!GetNextLine())
                    return false;
                auto parts = ParseVector(line);
                Manner_[std::stoi(parts[0])] = parts[1];
            }
        }
        else if (line == "Phonemes:")
        {
            if (!GetNextLine())
                return false;
            int count = std::stoi(line);
            for (int i = 0; i < count; ++i)
            {
                if (!GetNextLine())
                    return false;
                auto parts = ParseVector(line);
                Phonemes_[std::stoi(parts[0])] = parts[1];
            }
        }
        else if (line.find("Feature:") != std::string::npos || line.find("GridMap:") != std::string::npos)
        {
            std::string section = line;
            if (!GetNextLine())
                return false;
            int count = std::stoi(line);
            if (!GetNextLine())
                return false;
            auto data = ParseIntVector(line);
            for (int i = 0; i < count; ++i)
            {
                int baseIdx = i * 3;
                if (section == "PlaceFeature:")
                    PlaceFeatureRelations_[data[baseIdx]][data[baseIdx + 1]] = static_cast<FeatureState>(data[baseIdx + 2]);
                else if (section == "MannerFeature:")
                    MannerFeatureRelations_[data[baseIdx]][data[baseIdx + 1]] = static_cast<FeatureState>(data[baseIdx + 2]);
                else if (section == "PhonemeFeature:")
                    PhonemeFeatureRelations_[data[baseIdx]][data[baseIdx + 1]] = static_cast<FeatureState>(data[baseIdx + 2]);
                else if (section == "GridMap:")
                    MapPlaceMannerToPhoneme_[{data[baseIdx], data[baseIdx + 1]}] = data[baseIdx + 2];
            }
            if (section == "GridMap:")
                break; // PhonemeTable セクションの終了
        }
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
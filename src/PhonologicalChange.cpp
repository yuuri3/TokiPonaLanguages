#include "PhonologicalChange.h"

/**
 * @brief 変化前後の音韻アイテムの対応関係が正しいか判定する
 *
 * @return 判定結果（条件を満たせば true、満たさなければ false）
 */
bool PhonologicalChange::IsValid() const
{
    // 連続する Phoneme を1つにまとめた参照のリストを作成する
    std::vector<const PhoneticItem *> reducedBefore;
    for (const auto &item : BeforePhoneticItems_)
    {
        if (item.Type_ == PhoneticItemType::Phoneme)
        {
            if (reducedBefore.empty() || reducedBefore.back()->Type_ != PhoneticItemType::Phoneme)
            {
                reducedBefore.push_back(&item);
            }
        }
        else
        {
            reducedBefore.push_back(&item);
        }
    }

    std::vector<const PhoneticItem *> reducedAfter;
    for (const auto &item : AfterPhoneticItems_)
    {
        if (item.Type_ == PhoneticItemType::Phoneme)
        {
            if (reducedAfter.empty() || reducedAfter.back()->Type_ != PhoneticItemType::Phoneme)
            {
                reducedAfter.push_back(&item);
            }
        }
        else
        {
            reducedAfter.push_back(&item);
        }
    }

    // 要素数が一致しない場合は対応していない
    if (reducedBefore.size() != reducedAfter.size())
    {
        return false;
    }

    // 各要素の対応を検証する
    for (size_t i = 0; i < reducedBefore.size(); ++i)
    {
        // Type_ が一致しない場合
        if (reducedBefore[i]->Type_ != reducedAfter[i]->Type_)
        {
            return false;
        }

        // Feature の場合は ID の一致を検証する
        if (reducedBefore[i]->Type_ == PhoneticItemType::Feature)
        {
            if (reducedBefore[i]->ID_ != reducedAfter[i]->ID_)
            {
                return false;
            }
        }
    }

    return true;
}
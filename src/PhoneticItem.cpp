#include "PhoneticItem.h"

/**
 * @brief パラメータを指定してインスタンスを生成する
 * @param type 音韻要素の種別
 * @param id 音韻要素のID
 * @param state 素性の状態（+/-）
 * @return PhoneticItem 生成されたインスタンス
 */
PhoneticItem PhoneticItem::Create(const PhoneticItemType type, const int id, const FeatureState state)
{
    PhoneticItem result;
    result.Type_ = type;
    result.ID_ = id;
    result.State_ = state;
    return result;
}
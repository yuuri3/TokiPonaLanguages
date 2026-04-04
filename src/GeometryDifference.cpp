#include "GeometryDifference.h"

/**
 * @brief コンストラクタ
 *
 * @param operationType 操作の種類
 * @param targetRow 対象の行番号
 * @param targetColumn 対象の列番号
 * @param placeName 地点名（地点名変更時のみ使用）
 */
GeometryDifference::GeometryDifference(GeometryOperationType operationType, int targetRow, int targetColumn, const std::string &placeName)
    : operationType_(operationType), targetRow_(targetRow), targetColumn_(targetColumn), placeName_(placeName)
{
}

/**
 * @brief 操作の種類を取得する
 * @return GeometryOperationType 操作の種類
 */
GeometryOperationType GeometryDifference::GetOperationType() const
{
    return operationType_;
}

/**
 * @brief 対象の行番号を取得する
 * @return int 行番号
 */
int GeometryDifference::GetTargetRow() const
{
    return targetRow_;
}

/**
 * @brief 対象の列番号を取得する
 * @return int 列番号
 */
int GeometryDifference::GetTargetColumn() const
{
    return targetColumn_;
}

/**
 * @brief 地点名を取得する
 * @return std::string 地点名
 */
std::string GeometryDifference::GetPlaceName() const
{
    return placeName_;
}

/**
 * @brief 行に関する操作のインスタンスを生成する
 *
 * @param operationType 操作の種類
 * @param targetRow 対象の行番号
 * @return GeometryDifference 生成されたインスタンス
 */
GeometryDifference GeometryDifference::CreateRowOperation(GeometryOperationType operationType, int targetRow)
{
    return GeometryDifference(operationType, targetRow, -1, "");
}

/**
 * @brief 列に関する操作のインスタンスを生成する
 *
 * @param operationType 操作の種類
 * @param targetColumn 対象の列番号
 * @return GeometryDifference 生成されたインスタンス
 */
GeometryDifference GeometryDifference::CreateColumnOperation(GeometryOperationType operationType, int targetColumn)
{
    return GeometryDifference(operationType, -1, targetColumn, "");
}

/**
 * @brief 地点名変更操作のインスタンスを生成する
 *
 * @param targetRow 対象の行番号
 * @param targetColumn 対象の列番号
 * @param placeName 新しい地点名
 * @return GeometryDifference 生成されたインスタンス
 */
GeometryDifference GeometryDifference::CreateChangePlaceNameOperation(int targetRow, int targetColumn, const std::string &placeName)
{
    return GeometryDifference(GeometryOperationType::ChangePlaceName, targetRow, targetColumn, placeName);
}
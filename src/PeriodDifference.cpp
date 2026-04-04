#include "PeriodDifference.h"

/**
 * @brief コンストラクタ
 *
 * @param operationType 操作の種類
 * @param targetPeriod 対象の時代
 */
PeriodDifference::PeriodDifference(PeriodOperationType operationType, int targetPeriod)
    : operationType_(operationType), targetPeriod_(targetPeriod)
{
}

/**
 * @brief 操作の種類を取得する
 * @return PeriodOperationType 操作の種類
 */
PeriodOperationType PeriodDifference::GetOperationType() const
{
    return operationType_;
}

/**
 * @brief 対象の時代を取得する
 * @return int 時代
 */
int PeriodDifference::GetTargetPeriod() const
{
    return targetPeriod_;
}

/**
 * @brief 上に時代を追加する操作のインスタンスを生成する
 *
 * @param targetPeriod 対象の時代
 * @return PeriodDifference 生成されたインスタンス
 */
PeriodDifference PeriodDifference::CreateAddPeriodAboveOperation(int targetPeriod)
{
    return PeriodDifference(PeriodOperationType::AddPeriodAbove, targetPeriod);
}

/**
 * @brief 下に時代を追加する操作のインスタンスを生成する
 *
 * @param targetPeriod 対象の時代
 * @return PeriodDifference 生成されたインスタンス
 */
PeriodDifference PeriodDifference::CreateAddPeriodBelowOperation(int targetPeriod)
{
    return PeriodDifference(PeriodOperationType::AddPeriodBelow, targetPeriod);
}

/**
 * @brief 時代を削除する操作のインスタンスを生成する
 *
 * @param targetPeriod 対象の時代
 * @return PeriodDifference 生成されたインスタンス
 */
PeriodDifference PeriodDifference::CreateRemovePeriodOperation(int targetPeriod)
{
    return PeriodDifference(PeriodOperationType::RemovePeriod, targetPeriod);
}
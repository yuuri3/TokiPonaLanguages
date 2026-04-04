#pragma once

#include "stdafx.h"

enum class PeriodOperationType
{
    AddPeriodAbove,
    AddPeriodBelow,
    RemovePeriod
};

class PeriodDifference
{
private:
    PeriodOperationType operationType_;
    int targetPeriod_;

public:
    PeriodDifference(PeriodOperationType operationType, int targetPeriod);

    PeriodOperationType GetOperationType() const;
    int GetTargetPeriod() const;

    static PeriodDifference CreateAddPeriodAboveOperation(int targetPeriod);
    static PeriodDifference CreateAddPeriodBelowOperation(int targetPeriod);
    static PeriodDifference CreateRemovePeriodOperation(int targetPeriod);
};
#pragma once

#include "stdafx.h"

enum class GeometryOperationType
{
    ChangePlaceName,
    AddRowAbove,
    AddRowBelow,
    DeleteRow,
    AddColumnRight,
    AddColumnLeft,
    DeleteColumn
};

class GeometryDifference
{
private:
    GeometryOperationType operationType_;
    int targetRow_;
    int targetColumn_;
    std::string placeName_;

public:
    GeometryDifference(GeometryOperationType operationType, int targetRow, int targetColumn, const std::string &placeName);

    GeometryOperationType GetOperationType() const;
    int GetTargetRow() const;
    int GetTargetColumn() const;
    std::string GetPlaceName() const;

    static GeometryDifference CreateRowOperation(GeometryOperationType operationType, int targetRow);
    static GeometryDifference CreateColumnOperation(GeometryOperationType operationType, int targetColumn);
    static GeometryDifference CreateChangePlaceNameOperation(int targetRow, int targetColumn, const std::string &placeName);
};
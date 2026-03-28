#pragma once

#include <stdafx.h>
#include "Utility.h"

class GeometryTable
{
public:
    void Export(std::ofstream &file) const;
    bool Import(std::ifstream &file);

    void InitializeFromVector(const std::vector<std::vector<std::string>> &geography);
    std::vector<std::vector<std::string>> ToVector() const;

    void SetPlaceName(const int row, const int column, const std::string &name);
    void AddRowAbove(const int row);
    void AddRowBelow(const int row);
    void DeleteRow(const int row);
    void AddColumnRight(const int column);
    void AddColumnLeft(const int column);
    void DeleteColumn(const int column);

    bool Empty() const;

private:
    std::unordered_map<int, std::string> PlaceName_;
    std::unordered_map<std::pair<int, int>, int, PairHash> GridMap_;
};
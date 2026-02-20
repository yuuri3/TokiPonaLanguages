#pragma once

#include "stdafx.h"

std::vector<std::vector<std::string>> readCSV(const std::string &filename);
bool writeCSV(const std::string &filename, const std::vector<std::vector<std::string>> &data);
std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data);
std::vector<std::string> getNonEmptyStrings(const std::vector<std::vector<std::string>> &data);
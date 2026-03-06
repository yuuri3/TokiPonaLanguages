#pragma once

#include "stdafx.h"

static std::string APPLICATION_NAME = "うなぎエディタ";
static std::string APPLICATION_VERSION = "0.4.1";

std::vector<std::vector<std::string>> readCSV(const std::string &filename);
bool writeCSV(const std::string &filename, const std::vector<std::vector<std::string>> &data);
std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data);
std::vector<std::string> getNonEmptyStrings(const std::vector<std::vector<std::string>> &data);
std::string JoinStrs(const std::vector<std::string> &elements, const std::string &delimiter);
std::string EraseSpace(std::string str);

void DisplayTable(QTableWidget *table, const std::vector<std::vector<std::string>> &data, const bool IsEdit = false);
void ClearLayout(QLayout *layout);
void DeleteWidget(QWidget *widget);
void ClearWidget(QWidget *widget);
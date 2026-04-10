#pragma once
#include "stdafx.h"

struct TableData
{
    std::vector<std::string> Header;
    std::vector<std::vector<std::string>> Body;

    TableData Fill() const
    {
        TableData result = *this;
        result.Header.emplace_back("");
        for (auto &data : result.Body)
        {
            data.emplace_back("");
        }
        return result;
    }
};
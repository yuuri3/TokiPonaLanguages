#pragma once

#include <stdafx.h>
#include <PhoneticItemType.h>
#include <FeatureState.h>

struct PhoneticItem
{
    PhoneticItemType Type_;
    int ID_;
    FeatureState State_;

    static PhoneticItem Create(const PhoneticItemType type, const int id, const FeatureState state);
};
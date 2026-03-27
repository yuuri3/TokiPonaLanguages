#pragma once

#include <stdafx.h>
#include <FeatureState.h>
#include <PhoneticItem.h>
#include "Utility.h"

class PhonemeTable
{
public:
    std::string ConvertToString(const std::vector<int> &form) const;
    std::vector<int> ConvertToPhoneme(const std::string &phonemeIDs) const;
    bool Empty() const;
    bool Matches(const int phonemeId, const PhoneticItem &condition) const;
    void Export(std::ofstream &file) const;
    bool Import(std::ifstream &file);

    static PhonemeTable CreateDummyTable();

    std::string ConvertPhoneticItemToString(const std::vector<PhoneticItem> &phoneticItems) const;
    std::vector<PhoneticItem> ConvertStringToPhoneticItem(const std::string &str) const;

private:
    std::unordered_map<int, std::string> Features_;
    std::unordered_map<int, std::string> Places_;
    std::vector<int> PlaceOrder_;
    std::unordered_map<int, std::unordered_map<int, FeatureState>> PlaceFeatureRelations_;
    std::unordered_map<int, std::string> Manner_;
    std::vector<int> MannerOrder_;
    std::unordered_map<int, std::unordered_map<int, FeatureState>> MannerFeatureRelations_;
    std::unordered_map<int, std::string> Phonemes_;
    std::unordered_map<int, std::map<int, FeatureState>> PhonemeFeatureRelations_;
    std::unordered_map<std::pair<int, int>, int, PairHash> MapPlaceMannerToPhoneme_;
};
#include "stdafx.h"

class LanguageFamilySimulator;

std::optional<LanguageFamilySimulator> evolution(
    const int nLoanword,
    const double pPhonologicalChange,
    const double pSoundLoss,
    const std::string &protoLanguagePath,
    const std::string &phonemeTablePath,
    const std::string &geographyPath,
    const std::string &outputPath);
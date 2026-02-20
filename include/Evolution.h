#include "LanguageFamilySimulator.h"
#include <iostream>
#include <map>
#include <optional>

#include <iostream>

std::optional<LanguageFamilySimulator> evolution(
    const int nLoanword,
    const double pPhonologicalChange,
    const double pSoundLoss,
    const double pSemanticShift,
    const double maxSemanticShiftRate,
    const std::string &protoLanguagePath,
    const std::string &phonemeTablePath,
    const std::string &geographyPath,
    const std::string &outputPath);
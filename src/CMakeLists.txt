add_library(src
  SHARED
    LanguageDifference.cpp
    LanguageFamily.cpp
    LanguageFamilySimulator.cpp
    Meaning.cpp
    PhonemeConverter.cpp
    Random.cpp
    Utility.cpp
    Word.cpp
    WordForSimulation.cpp
  )

target_include_directories(src
  PUBLIC C:/TokiLanguages/include
  )
# src/CMakeLists.txt

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ヘッダーファイルもリストに含める
add_library(src
  SHARED
    EditGeometryDialog.cpp
    EditLanguageWindow.cpp
    EditPeriodDialog.cpp
    EditPhonologicalChangeDialog.cpp
    EditWordDialog.cpp
    Evolution.cpp
    HelpDialog.cpp
    Language.cpp
    LanguageDifference.cpp
    LanguageFamily.cpp
    LanguageFamilySimulator.cpp
    MainWindow.cpp
    Phoneme.cpp
    PhonemeConverter.cpp
    Random.cpp
    SimulationDialog.cpp
    UnimplementedDialog.cpp
    Utility.cpp
    Word.cpp
    ../include/EditGeometryDialog.h
    ../include/EditLanguageWindow.h
    ../include/EditPeriodDialog.h
    ../include/EditPhonologicalChangeDialog.h
    ../include/EditWordDialog.h
    ../include/Evolution.h
    ../include/HelpDialog.h
    ../include/Language.h
    ../include/LanguageDifference.h
    ../include/LanguageDifferenceType.h
    ../include/LanguageFamily.h
    ../include/LanguageFamilySimulator.h
    ../include/MainWindow.h
    ../include/Phoneme.h
    ../include/PhonemeConverter.h
    ../include/PhoneticEnvironment.h
    ../include/Random.h
    ../include/SimulationDialog.h
    ../include/UnimplementedDialog.h
    ../include/Utility.h
    ../include/Word.h
  )

target_include_directories(src
  PUBLIC C:/TokiLanguages/include
  )

target_link_libraries(src PUBLIC Qt6::Widgets)
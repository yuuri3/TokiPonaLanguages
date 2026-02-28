# src/CMakeLists.txt

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ヘッダーファイルもリストに含める
add_library(src
  SHARED
    Evolution.cpp
    LanguageDifference.cpp
    Language.cpp
    LanguageFamily.cpp
    LanguageFamilySimulator.cpp
    Phoneme.cpp
    PhonemeConverter.cpp
    Random.cpp
    Utility.cpp
    Word.cpp
    MainWindow.cpp
    UnimplementedDialog.cpp
    SimulationDialog.cpp
    EditLanguageWindow.cpp
    EditPeriodDialog.cpp
    EditGeometryDialog.cpp
    EditWordDialog.cpp
    ../include/Language.h
    ../include/LanguageFamilySimulator.h
    ../include/Phoneme.h
    ../include/PhonemeConverter.h
    ../include/Utility.h
    ../include/MainWindow.h
    ../include/UnimplementedDialog.h
    ../include/SimulationDialog.h
    ../include/Evolution.h
    ../include/EditLanguageWindow.h
    ../include/EditPeriodDialog.h
    ../include/EditGeometryDialog.h
    ../include/EditWordDialog.h
  )

target_include_directories(src
  PUBLIC C:/TokiLanguages/include
  )

target_link_libraries(src PUBLIC Qt6::Widgets)
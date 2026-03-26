# src/CMakeLists.txt

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ヘッダーファイルもリストに含める
add_library(src
  SHARED
    DialogLayout.cpp
    EditGeometryDialog.cpp
    EditLanguageWindow.cpp
    EditPeriodDialog.cpp
    EditPhonologicalChangeDialog.cpp
    EditWordDialog.cpp
    GeometryTable.cpp
    HelpDialog.cpp
    Language.cpp
    LanguageDifference.cpp
    LanguageFamily.cpp
    MainWindow.cpp
    PhonemeTable.cpp
    PhoneticItem.cpp
    Random.cpp
    SelectLanguageDialog.cpp
    UnimplementedDialog.cpp
    Utility.cpp
    Word.cpp
    ../include/DialogLayout.h
    ../include/EditGeometryDialog.h
    ../include/EditLanguageWindow.h
    ../include/EditPeriodDialog.h
    ../include/EditPhonologicalChangeDialog.h
    ../include/EditWordDialog.h
    ../include/FeatureState.h
    ../include/GeometryTable.h
    ../include/HelpDialog.h
    ../include/Language.h
    ../include/LanguageDifference.h
    ../include/LanguageDifferenceType.h
    ../include/LanguageFamily.h
    ../include/MainWindow.h
    ../include/PhonemeTable.h
    ../include/PhoneticItem.h
    ../include/PhoneticItemType.h
    ../include/Random.h
    ../include/SelectLanguageDialog.h
    ../include/UnimplementedDialog.h
    ../include/Utility.h
    ../include/Word.h
  )

target_include_directories(src
  PUBLIC C:/TokiLanguages/include
  )

target_link_libraries(src PUBLIC Qt6::Widgets)
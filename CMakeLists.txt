cmake_minimum_required(VERSION 3.16) # Qt6は3.16以上を推奨
project(TokiLanguages CXX)

# Qtの自動処理を有効化（重要）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt6のWidgetsモジュールを探す
find_package(Qt6 REQUIRED COMPONENTS Widgets)

include_directories(include)
add_subdirectory(src)

# 1. バージョン定義
set(APP_VERSION "0.3.7")
set(EXE_VERSION "0.3.7.0")
string(REPLACE "." "," APP_VERSION_COMMA "${APP_VERSION},0") 
string(REPLACE "." "," EXE_VERSION_COMMA "${EXE_VERSION}") 

# 2. テンプレートから実際の .rc ファイルを生成
configure_file(version.rc.in version.rc @ONLY)

set(RESOURCE_FILES build/version.rc)

if(WIN32)
    set(UI_RESOURCES "resource.rc")
    add_executable(Unagi-Conlang-Editor WIN32 main.cpp ${UI_RESOURCES} ${RESOURCE_FILES})
else()
    add_executable(Unagi-Conlang-Editor WIN32 main.cpp ${RESOURCE_FILES})
endif()

# 生成されるファイル名（.exe）を日本語にする設定を追加
set_target_properties(Unagi-Conlang-Editor PROPERTIES OUTPUT_NAME "うなぎエディタ_Ver${APP_VERSION}")

# リンク設定も英字のターゲット名に合わせる
target_link_libraries(Unagi-Conlang-Editor PRIVATE Qt6::Widgets)

# Qtのライブラリをリンク
target_link_libraries(Unagi-Conlang-Editor PRIVATE src Qt6::Widgets)

# ビルド直後に、exeの隣にDLLを集める設定
add_custom_command(TARGET Unagi-Conlang-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:src> $<TARGET_FILE_DIR:Unagi-Conlang-Editor>
    COMMAND ${CMAKE_COMMAND} -D "EXE=$<TARGET_FILE:Unagi-Conlang-Editor>" -P "${CMAKE_SOURCE_DIR}/fixup.cmake"
    COMMENT "Fixing up dependencies..."
)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
add_custom_command(TARGET Unagi-Conlang-Editor POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} "$<TARGET_FILE:Unagi-Conlang-Editor>"
    COMMENT "Running windeployqt..."
)
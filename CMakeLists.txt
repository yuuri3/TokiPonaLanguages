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
if(WIN32)
    set(UI_RESOURCES "resource.rc")
    add_executable(a.out WIN32 main.cpp ${UI_RESOURCES})
else()
    add_executable(a.out WIN32 main.cpp)
endif()

# Qtのライブラリをリンク
target_link_libraries(a.out PRIVATE src Qt6::Widgets)

# ビルド直後に、exeの隣にDLLを集める設定
add_custom_command(TARGET a.out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:src> $<TARGET_FILE_DIR:a.out>
    COMMAND ${CMAKE_COMMAND} -D "EXE=$<TARGET_FILE:a.out>" -P "${CMAKE_SOURCE_DIR}/fixup.cmake"
    COMMENT "Fixing up dependencies..."
)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
add_custom_command(TARGET a.out POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} "$<TARGET_FILE:a.out>"
    COMMENT "Running windeployqt..."
)
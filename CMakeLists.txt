cmake_minimum_required(VERSION 3.13)
project(test_cmake CXX)

if(MINGW OR WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()

include_directories(include)
add_subdirectory(src)

add_executable(a.out main.cpp)
target_link_libraries(a.out PRIVATE src)

# ビルド完了後、libsrc.dll を exe と同じ場所にコピーする設定
add_custom_command(TARGET a.out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:src>
    $<TARGET_FILE_DIR:a.out>
)

# 実行ファイル(a.out)のビルド後に、依存DLLを自動で収集する
# fixup_bundle という標準機能を使用します
install(TARGETS a.out DESTINATION .)

# ビルド直後に、exeの隣にDLLを集める設定
add_custom_command(TARGET a.out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:src> $<TARGET_FILE_DIR:a.out>
    COMMAND ${CMAKE_COMMAND} -D "EXE=$<TARGET_FILE:a.out>" -P "${CMAKE_SOURCE_DIR}/fixup.cmake"
    COMMENT "Fixing up dependencies..."
)
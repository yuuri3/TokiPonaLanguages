# fixup.cmake
include(BundleUtilities)

# 検索対象のパス（MSYS2のbinフォルダ）を自動特定
get_filename_component(MSYS_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

# exeが必要とするDLLを自動で探し、コピーする
fixup_bundle("${EXE}" "" "${MSYS_BIN_DIR}")
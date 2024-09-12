cmake_minimum_required(VERSION 3.13)

project(xyts-strategy-sdk VERSION 1.0.0 LANGUAGES C CXX)

add_library(xyts-strategy-sdk INTERFACE)
target_include_directories(xyts-strategy-sdk INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/third_party/nlohmann_json/include"
    "${PROJECT_SOURCE_DIR}/third_party/fmt/include"
    "${PROJECT_SOURCE_DIR}/third_party/quill/include")
target_compile_definitions(xyts-strategy-sdk INTERFACE QUILL_DISABLE_NON_PREFIXED_MACROS QUILL_FMT_EXTERNAL)


cmake_minimum_required(VERSION 3.15)

project(compact CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser src/parser.y "${CMAKE_CURRENT_BINARY_DIR}/parser.cc")

flex_target(lexer src/lexer.l "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc")

add_executable(compact
src/error.cpp
    src/main.cpp
    src/interpreter/value.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/parser.cc"
)

target_include_directories(compact PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_BINARY_DIR}"
)

# Ref: https://stackoverflow.com/a/60890947/11553216
# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if((MSVC) AND(MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options(compact PUBLIC "/Zc:__cplusplus")
endif()

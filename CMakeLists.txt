cmake_minimum_required(VERSION 3.1)
project(AoC2023)


add_executable(day1 day1.cc)
set_target_properties(day1 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_compile_options(day1 PRIVATE -Wall -Wextra -Wpedantic)
endif()
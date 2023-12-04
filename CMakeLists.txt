cmake_minimum_required(VERSION 3.1)
project(AoC2023)


add_executable(day1 day1.cc)
set_target_properties(day1 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
add_executable(day3 day3.cc)
set_target_properties(day3 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
add_executable(day4 day4.cc)
set_target_properties(day4 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_compile_options(day1 PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
target_compile_options(day3 PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
target_compile_options(day4 PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
endif()
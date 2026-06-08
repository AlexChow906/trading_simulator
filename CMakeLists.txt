cmake_minimum_required(VERSION 3.20)
project(low_latency_trading_engine LANGUAGES CXX)

# Enforce C++20, no compiler extensions (strict standard compliance)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for clangd / IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Strict warnings — catch bugs at compile time, not runtime
add_compile_options(-Wall -Wextra -Wpedantic)

# Debug: enable AddressSanitizer + UndefinedBehaviorSanitizer
# ASan catches: buffer overflows, use-after-free, memory leaks
# UBSan catches: signed integer overflow, null dereference, alignment issues
# -fno-omit-frame-pointer: preserves stack traces for better error reporting
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Release: -O2 for predictable latency, -march=native for CPU-specific optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native)
endif()

# Main executable
add_executable(trading_engine src/main.cpp)
add_library(order_book src/order_book/order_book.cpp src/matching_engine/matching_engine.cpp)
target_include_directories(order_book PUBLIC ${CMAKE_SOURCE_DIR}/include)

# --- Testing ---
# FetchContent downloads and builds GoogleTest as part of our project
# No system install needed — anyone can clone and build
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(unit_tests tests/order_test.cpp tests/matching_engine_test.cpp)
target_link_libraries(unit_tests GTest::gtest_main order_book)
add_test(NAME unit_tests COMMAND unit_tests)

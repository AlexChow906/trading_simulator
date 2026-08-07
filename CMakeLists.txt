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
add_library(order_book src/order_book/order_book.cpp src/matching_engine/matching_engine.cpp src/matching_engine/engine_runner.cpp)
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
add_executable(unit_tests tests/order_test.cpp tests/matching_engine_test.cpp tests/thread_safe_queue_test.cpp
    tests/engine_runner_test.cpp tests/pool_allocator_test.cpp)
target_link_libraries(unit_tests GTest::gtest_main order_book)
add_test(NAME unit_tests COMMAND unit_tests)

# --- Benchmarking ---
# Google Benchmark, fetched the same way as GoogleTest
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
)
# Don't build Google Benchmark's own test suite — faster builds, avoids a second googletest copy
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(benchmarks benchmarks/order_book_benchmark.cpp)
target_link_libraries(benchmarks benchmark::benchmark order_book)

add_executable(latency_benchmark benchmarks/latency_benchmark.cpp)
target_link_libraries(latency_benchmark order_book)

add_executable(pool_benchmark benchmarks/pool_allocator_benchmark.cpp)
target_link_libraries(pool_benchmark benchmark::benchmark order_book)

# --- Networking ---
# Asio is header-only — no separate build step, just fetch and expose the headers
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)

add_library(gateway src/gateway/gateway.cpp src/gateway/session.cpp)
target_link_libraries(gateway order_book)
target_include_directories(gateway PUBLIC ${CMAKE_SOURCE_DIR}/include ${asio_SOURCE_DIR}/asio/include)

add_executable(gateway_server src/main.cpp)
target_link_libraries(gateway_server gateway)

# --- Strategy agents ---
add_library(strategy src/strategy/agent.cpp src/strategy/market_maker.cpp src/strategy/execution_agent.cpp src/strategy/momentum_agent.cpp src/strategy/mean_reversion_agent.cpp)
target_link_libraries(strategy order_book)
target_include_directories(strategy PUBLIC ${CMAKE_SOURCE_DIR}/include ${asio_SOURCE_DIR}/asio/include)

add_executable(simulator src/simulator.cpp)
target_link_libraries(simulator strategy gateway)

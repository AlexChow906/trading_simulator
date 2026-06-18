# Low-Latency Trading Engine

A C++20 low-latency electronic trading engine simulator featuring an order matching engine, limit order book, and market data distribution — built with systems programming and performance engineering principles.

## Build

**Requirements:** CMake 3.20+, Ninja, C++20 compiler (Clang recommended)

All library dependencies (GoogleTest, Google Benchmark, fmt, spdlog, Boost) are fetched automatically via CMake FetchContent.

```bash
# Debug build (with AddressSanitizer + UBSanitizer)
cmake -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug

# Release build (with -O2 -march=native)
cmake -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
```

## Run

```bash
./build/debug/trading_engine
```

## Test

```bash
ctest --test-dir build/debug --output-on-failure
```

## Benchmark

Benchmarks must be built in Release mode — Debug builds include sanitizers and no optimization, so the numbers are meaningless.

```bash
# Build the Release benchmarks
cmake -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release

# Throughput (Google Benchmark): order insertion, cancellation, matching
./build/release/benchmarks

# Latency distribution (manual harness): p50 / p99 / p99.9
./build/release/latency_benchmark
```

## Project Structure

```
src/              # Implementation files
  matching_engine/
  order_book/
  gateway/
  market_data/
include/          # Header files (public interfaces)
  matching_engine/
  order_book/
  gateway/
  market_data/
tests/            # Unit tests (GoogleTest)
benchmarks/       # Performance benchmarks (Google Benchmark)
docs/             # Documentation and architecture diagrams
```

## Roadmap

- [x] Phase 1 — Project setup, build system, test infrastructure
- [x] Phase 2 — Core domain model (Order, Side, OrderType)
- [x] Phase 3 — Limit order book (bid/ask books, price levels, FIFO matching)
- [x] Phase 4 — Matching engine (limit/market orders, partial fills, trade generation)
- [x] Phase 5 — Benchmarking (latency distributions, throughput measurement)
- [ ] Phase 6 — Multithreading (producer/consumer queues, dedicated threads)
- [ ] Phase 7 — Networking (TCP gateway via Boost.Asio)
- [ ] Phase 8 — Optimization (custom allocators, cache alignment, profiling)
- [ ] Phase 9 — Polish (Dockerfile, CI, architecture diagrams)

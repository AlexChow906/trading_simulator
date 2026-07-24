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
- [x] Phase 6 — Multithreading (producer/consumer queues, dedicated threads)
- [x] Phase 7 — Networking (TCP gateway via Boost.Asio)
- [x] Phase 8 — Optimization (custom allocators, cache alignment, profiling)
- [ ] Phase 9 — Polish (Dockerfile, CI, architecture diagrams)
- [ ] Phase 10 — Strategy agent layer (market-making + execution agents trading against the engine over the gateway)

### Phase 10 — Strategy agents (stretch goal)

The phases above build *exchange infrastructure* (the venue where orders meet). Phase 10 adds the *participant* side: independent agents that connect to the engine via the Phase 7 gateway, consume the market-data feed, and submit orders — turning the simulator into a full market with participants trading against each other.

- **Market-making agent** — continuously quotes a bid and an ask, earns the spread, and manages inventory risk (e.g. Avellaneda–Stoikov). Fits naturally since a market maker operates directly on the order book.
- **Execution agents** — momentum and mean-reversion strategies that react to the market-data feed and send marketable orders.
- **Metrics** — per-agent P&L, fill rates, inventory over time.

Research-flavoured, statistics-heavy models (statistical arbitrage, pairs trading, options pricing, Monte Carlo risk/VaR) are intentionally **out of scope** — they belong in a separate strategy-research project to keep this repo focused on low-latency systems.

# Benchmark Results & Optimization Notes

Baseline performance of the order book and matching engine, captured at the end of Phase 5. These numbers establish a reference point so later optimization work (Phase 8) can be measured against it.

## Methodology

- **Build:** Release mode (`-O2 -march=native`), no sanitizers.
- **Throughput:** Google Benchmark (`benchmarks` target). Each operation runs millions of times; the framework reports average time per op. Destructive operations (cancel, match) consume state, so setup is refilled in batches of 4096 with timing paused — the pause cost is amortized to near-zero per op.
- **Latency:** Manual harness (`latency_benchmark` target). Each operation is individually timed with `std::chrono::steady_clock`; 1,000,000 samples are collected, sorted, and percentiles read off directly. Clock overhead is calibrated separately (two back-to-back `now()` calls) and subtracted.
- **Machine:** Apple Silicon (arm64), macOS. Absolute numbers are machine-dependent; the *relationships* between them are what matter.

## Throughput (average time per operation)

| Operation | Time | Throughput |
|-----------|------|------------|
| `add_order` | ~27 ns | ~36 M ops/sec |
| `cancel_order` | ~25 ns | ~39 M ops/sec |
| `match_order` (one fill) | ~48 ns | ~21 M ops/sec |

**Reading it:**
- Cancel is marginally cheaper than add — cancel is a hash lookup + one `std::list` erase (a single deallocation), while add is a map lookup + a list-node allocation + a hash insert.
- Matching is ~2× the cost of a plain insert: it walks the book, constructs a `Trade`, decrements quantities, and removes the filled resting order from all three structures (list erase + `std::map` erase + `unordered_map` erase).

## Latency distribution (`add_order`, isolated per-op timing)

| Percentile | Latency |
|------------|---------|
| Clock overhead (calibrated) | ~0 ns (fast fine-grained timer) |
| p50 | ~41 ns |
| p99 | ~42 ns |
| p99.9 | ~1100 ns |

**Reading it:**
- **p50 (41 ns) > throughput add (27 ns)** even though clock overhead is ~0. The gap is *instruction-level parallelism*: the tight throughput loop lets the CPU pipeline consecutive `add_order` calls, whereas wrapping each op in two `now()` reads serializes them and measures each in isolation. Both numbers are valid — throughput answers "sustained load," latency answers "one order on an idle book."
- **p99 ≈ p50.** The common case is extremely stable — 99% of operations land within ~1 ns of the median.
- **p99.9 ≈ 1100 ns — the tail.** ~26× the median. This is caused by heap activity: an occasional `std::list` node allocation hitting a slow allocator path, or the `unordered_map` crossing its load factor and **rehashing** the whole table. One unlucky order pays the full cost. In a live system this is the order that misses the market.

## Phase 8 — Optimization results

### Pool allocator + intrusive list

Replaced `std::list<Order>` (per-node heap allocation) with:
- **Pool allocator** — pre-allocates a contiguous block of Order slots at startup. Allocate/deallocate are O(1) free-list operations with no OS calls.
- **Intrusive doubly-linked list** — prev/next pointers live inside the `Order` struct itself, so the list never allocates.

**Pool allocator microbenchmark** (isolated alloc + dealloc cycle):

| Method | Time |
|--------|------|
| `new` / `delete` | 22.5 ns |
| Pool allocator | 9.35 ns |

**2.4x faster** — and critically, no variance from OS allocator slow paths.

**Latency distribution (`add_order`) — before vs after:**

| Percentile | Before (std::list + heap) | After (pool + intrusive list) | Change |
|------------|--------------------------|-------------------------------|--------|
| p50 | 1.0 ns | 1.0 ns | — |
| p99 | 43.0 ns | 42.0 ns | — |
| p99.9 | 2042.0 ns | 1334.0 ns | **-35%** |

The tail latency (p99.9) dropped from ~2µs to ~1.3µs. The median and p99 are unchanged — those were already fast enough that allocation wasn't the bottleneck.

### Remaining tail latency sources

The residual ~1.3µs p99.9 is driven by:
- **`std::map` rebalancing** — red-black tree rotations when inserting new price levels
- **`std::unordered_map` rehashing** — when the order lookup table crosses its load factor
- **Cache misses** — `std::map` nodes are scattered across the heap

Further optimization would target these with `reserve()` on the hash map, an open-addressing hash table, or a flat sorted structure for price levels.

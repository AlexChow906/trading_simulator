#include <benchmark/benchmark.h>
#include <vector>

#include "concurrency/pool_allocator.h"
#include "order_book/order.h"

static void BM_HeapAlloc(benchmark::State& state) {
  for (auto _ : state) {
    Order* ptr = new Order{1, Side::Buy, OrderType::Limit, 100.00, 10, 0};
    delete ptr;
    benchmark::DoNotOptimize(ptr);
  }
}

static void BM_PoolAlloc(benchmark::State& state) {
  PoolAllocator<Order, 1> pool;
  for (auto _ : state) {
    Order* ptr = pool.allocate();
    *ptr = Order{1, Side::Buy, OrderType::Limit, 100.00, 10, 0};
    pool.deallocate(ptr);
    benchmark::DoNotOptimize(ptr);
  }
}

BENCHMARK(BM_HeapAlloc);
BENCHMARK(BM_PoolAlloc);

BENCHMARK_MAIN();

#include <benchmark/benchmark.h>
#include <vector>

#include "order_book/order_book.h"
#include "order_book/order.h"
#include "order_book/types.h"
#include "matching_engine/matching_engine.h"

static void BM_AddOrder(benchmark::State& state) {
  OrderBook book;
  uint64_t id = 0;

  for (auto _ : state) {
    Order order{id++, Side::Buy, OrderType::Limit, 100.00, 10, 0};
    book.add_order(order);
    benchmark::DoNotOptimize(book);
  }
}

static void BM_CancelOrder(benchmark::State& state) {
  OrderBook book;
  uint64_t id = 0;
  std::vector<uint64_t> ids;
  const auto batch_size = 4096;

  for (auto _ : state) {
    if (ids.empty()) {
      state.PauseTiming();

      for (int i = 0; i < batch_size; ++i) {
        auto batch_order_id = id;
        Order order{id++, Side::Buy, OrderType::Limit, 100.00, 10, 0};
        ids.push_back(batch_order_id);
        book.add_order(order);
      }

      state.ResumeTiming();
    }

    uint64_t order_id = ids.back();
    ids.pop_back();
    book.cancel_order(order_id);
    benchmark::DoNotOptimize(book);
  }
}

static void BM_MatchOrder(benchmark::State& state) {
  MatchingEngine engine;
  uint64_t id = 0;
  uint64_t remaining = 0;
  const auto batch_size = 4096;

  for (auto _ : state) {
    if (remaining == 0) {
      state.PauseTiming();

      for (int i = 0; i < batch_size; ++i) {
        Order order{id++, Side::Sell, OrderType::Limit, 100.00, 1, 0};
        engine.process_order(order);
      }

      remaining = batch_size;
      state.ResumeTiming();
    }

    Order order{id++, Side::Buy, OrderType::Limit, 100.00, 1, 0};
    engine.process_order(order);
    benchmark::DoNotOptimize(engine);
    remaining--;
  }
}
BENCHMARK(BM_AddOrder);
BENCHMARK(BM_CancelOrder);
BENCHMARK(BM_MatchOrder);

BENCHMARK_MAIN();

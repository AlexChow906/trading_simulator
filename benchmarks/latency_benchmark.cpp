#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdio>

#include "order_book/order_book.h"
#include "order_book/order.h"
#include "order_book/types.h"

int main() {
  const int N = 1'000'000;
  std::vector<double> add_latencies;
  add_latencies.reserve(N);

  const int calibration_iters = 100'000;
  std::vector<double> clock_overheads;
  clock_overheads.reserve(calibration_iters);

  OrderBook book;
  uint64_t id = 0;

  for (int i = 0 ; i < calibration_iters; ++i) {
    auto t0 = std::chrono::steady_clock::now();
    auto t1 = std::chrono::steady_clock::now();

    double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    clock_overheads.push_back(elapsed);
  }

  for (int i = 0; i < N; ++i) {
    Order order{id++, Side::Buy, OrderType::Limit, 100.00, 10, 0};

    auto t0 = std::chrono::steady_clock::now();
    book.add_order(order);
    auto t1 = std::chrono::steady_clock::now();

    double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    add_latencies.push_back(elapsed);
  }

  std::sort(add_latencies.begin(), add_latencies.end());
  std::sort(clock_overheads.begin(), clock_overheads.end());
  double med_clock_overhead = clock_overheads[size_t(calibration_iters * 0.50)];
  double p50 = add_latencies[size_t(N * 0.50)] - med_clock_overhead;
  double p99 = add_latencies[size_t(N * 0.99)] - med_clock_overhead;
  double p99_9 = add_latencies[size_t(N * 0.999)] - med_clock_overhead;
  printf("clock overhead: %.1f ns\n", med_clock_overhead);
  printf("add_order p50: %.1f ns; p99: %.1f ns; p99.9: %.1f ns\n", p50, p99, p99_9);
}

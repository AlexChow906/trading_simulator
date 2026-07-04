#include <cstdio>

#include "matching_engine/engine_runner.h"

void EngineRunner::start() {
  matching_thread_ = std::thread(&EngineRunner::matching_loop, this);
  market_data_thread_ = std::thread(&EngineRunner::market_data_loop, this);
}

void EngineRunner::stop() {
  in_queue_.stop();
  if (matching_thread_.joinable()) {
    matching_thread_.join();
  }

  trades_queue_.stop();
  if (market_data_thread_.joinable()) {
    market_data_thread_.join();
  }
}

void EngineRunner::submit(Order order) {
  in_queue_.push(std::move(order));
}

uint64_t EngineRunner::orders_processed() const {
  return orders_processed_.load();
}

void EngineRunner::matching_loop() {
  while (true) {
    auto order = in_queue_.pop();
    if (!order) {
      break;
    }
    std::vector<Trade> trades = engine_.process_order(*order);
    for (const auto& trade: trades) {
      trades_queue_.push(trade);
    }
    orders_processed_++;
  }
}

void EngineRunner::market_data_loop() {
  while (true) {
    auto trade = trades_queue_.pop();
    if (!trade) {
      break;
    }
    printf("Trade id: %llu, price: %2f, quantity: %d", trade->trade_id, trade->price, trade->quantity);
  }
}

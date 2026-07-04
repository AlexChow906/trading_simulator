#pragma once

#include <atomic>
#include <thread>

#include "matching_engine/matching_engine.h"
#include "matching_engine/trade.h"
#include "concurrency/thread_safe_queue.h"
#include "order_book/order.h"

class EngineRunner {
  public:
    void start();
    void stop();
    void submit(Order order);
    uint64_t orders_processed() const;

  private:
    MatchingEngine engine_;
    ThreadSafeQueue<Order> in_queue_;
    ThreadSafeQueue<Trade> trades_queue_;
    std::thread matching_thread_;
    std::thread market_data_thread_;
    std::atomic<uint64_t> orders_processed_{0};

    void matching_loop();
    void market_data_loop();
};

#pragma once

#include <vector>
#include <cstdint>
#include "trade.h"
#include "order_book/order_book.h"

class MatchingEngine {
  public:
    std::vector<Trade> process_order(Order order);
    std::optional<double> best_bid() const { return book_.best_bid(); }
    std::optional<double> best_ask() const { return book_.best_ask(); }

  private:
    OrderBook book_;
    uint64_t next_trade_id_{1};
};

#pragma once

#include <map>
#include <unordered_map>
#include <list>
#include <optional>

#include "types.h"
#include "order.h"
#include "price_level.h"

class OrderBook{
  public:
    void add_order(const Order& order);
    bool cancel_order(uint64_t order_id);
    std::optional<double> best_bid() const;
    std::optional<double> best_ask() const;

  private:
    std::map<double, PriceLevel> bids_;
    std::map<double, PriceLevel> asks_;
    std::unordered_map<uint64_t, std::list<Order>::iterator> order_lookup_;
};

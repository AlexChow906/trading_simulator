#include "order_book/order_book.h"

void OrderBook::add_order(const Order& order) {
  auto& book = (order.side == Side::Buy) ? bids_ : asks_;
  auto& level = book[order.price];

  level.orders.push_back(order);
  order_lookup_[order.order_id] = std::prev(level.orders.end());
}

bool OrderBook::cancel_order(uint64_t order_id) {
  auto it = order_lookup_.find(order_id);
  if (it == order_lookup_.end()) {
    return false;
  }

  auto order_it = it->second;
  auto& book = (order_it->side == Side::Buy) ? bids_ : asks_;
  double price = order_it->price;

  book[price].orders.erase(order_it);
  if(book[price].orders.empty()) {
    book.erase(price);
  }

  order_lookup_.erase(it);
  return true;
}

std::optional<double> OrderBook::best_bid() const {
  if (bids_.empty()) {
    return std::nullopt;
  }

  return bids_.rbegin()->first;
}

std::optional<double> OrderBook::best_ask() const {
  if (asks_.empty()) {
    return std::nullopt;
  }

  return asks_.begin()->first;
}

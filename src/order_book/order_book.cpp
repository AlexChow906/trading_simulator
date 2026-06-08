#include "order_book/order_book.h"
#include "matching_engine/trade.h"

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

std::vector<Trade> OrderBook::match_order(Order& order, uint64_t& next_trade_id) {
  std::vector<Trade> trades;

  auto buy_side = order.side == Side::Buy;
  auto& book = buy_side ? asks_ : bids_;

  while (order.quantity > 0 && !(book.empty())) {
    auto best_price_level = buy_side ? book.begin() : std::prev(book.end());

    bool acceptable_price = (order.type == OrderType::Market ||
        (order.type == OrderType::Limit && order.side == Side::Buy && order.price >= best_price_level->first) ||
        (order.type == OrderType::Limit && order.side == Side::Sell && order.price <= best_price_level->first));
    if (!acceptable_price) break;

    auto& front_order = best_price_level->second.orders.front();
    auto fill_quantity = std::min(order.quantity, front_order.quantity);

    auto buy_order_id = buy_side ? order.order_id : front_order.order_id;
    auto sell_order_id = buy_side ? front_order.order_id : order.order_id;
    Trade trade = Trade(next_trade_id++, buy_order_id, sell_order_id, front_order.price, fill_quantity, order.timestamp);

    trades.push_back(trade);

    order.quantity = order.quantity - fill_quantity;
    front_order.quantity = front_order.quantity - fill_quantity;

    if (front_order.quantity == 0) {
      order_lookup_.erase(front_order.order_id);
      best_price_level->second.orders.pop_front();

      if (best_price_level->second.orders.empty()) {
        book.erase(best_price_level);
      }
    }
  }

  return trades;
}

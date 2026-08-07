#include <cstdio>

#include "strategy/mean_reversion_agent.h"

MeanReversionAgent::MeanReversionAgent(const std::string& host, uint16_t port, std::string name, uint32_t quantity, size_t window_size, double threshold, uint64_t start_id)
    : Agent(host, port, name), quantity_(quantity), window_size_(window_size), threshold_(threshold), start_id_(start_id) {
      next_order_id_ = start_id;
      start_id_ = start_id;
    }

void MeanReversionAgent::on_connected() {
  printf("[%s] Mean reversion agent connected\n", name_.c_str());
}

void MeanReversionAgent::on_trade(const TradeMessage& trade) {
  trade_prices_.push_back(trade.price);

  if (trade.buy_order_id >= start_id_ && trade.buy_order_id < start_id_ + 1000000) {
    inventory_ += trade.quantity;
    cash_ -= trade.price * trade.quantity;
    fills_received_++;
    printf("[%s] Trade %llu | price: %.2f qty: %u | inv: %d cash: %.2f\n",
      name_.c_str(), trade.trade_id, trade.price, trade.quantity, inventory_, cash_);
  }

  if (trade.sell_order_id >= start_id_ && trade.sell_order_id < start_id_ + 1000000) {
    inventory_ -= trade.quantity;
    cash_ += trade.price * trade.quantity;
    fills_received_++;
    printf("[%s] Trade %llu | price: %.2f qty: %u | inv: %d cash: %.2f\n",
      name_.c_str(), trade.trade_id, trade.price, trade.quantity, inventory_, cash_);
  }


  if (trade_prices_.size() > window_size_) {
    trade_prices_.pop_front();
  } else if (trade_prices_.size() < window_size_) {
    return;
  }

  double total_prices = 0.0;
  for (double price : trade_prices_) {
    total_prices += price;
  }
  double average_price = total_prices / trade_prices_.size();

  if (average_price - trade_prices_.back() > threshold_) {
    send_order(0, 0, trade_prices_.back(), quantity_);
  }

  if (trade_prices_.back() - average_price > threshold_) {
    send_order(1, 0, trade_prices_.back(), quantity_);
  }
}

#include <cstdio>

#include "strategy/execution_agent.h"

ExecutionAgent::ExecutionAgent(const std::string& host, uint16_t port, double target_price, uint32_t quantity, bool buy_side, uint32_t interval_ms, uint64_t start_id, std::string name)
    : Agent(host, port), target_price_(target_price), quantity_(quantity), buy_side_(buy_side), interval_ms_(interval_ms), timer_(io_context()), name_(name) {
      next_order_id_ = start_id;
      start_id_ = start_id;
    }

void ExecutionAgent::on_connected() {
  schedule_order();
}

void ExecutionAgent::on_trade(const TradeMessage& trade) {
  bool my_trade;
  if (buy_side_) {
      my_trade = (trade.buy_order_id >= start_id_ && trade.buy_order_id < start_id_ + 1000000);
  } else {
      my_trade = (trade.sell_order_id >= start_id_ && trade.sell_order_id < start_id_ + 1000000);
  }
  if (!my_trade) return;

  if (buy_side_) {
    inventory_ += trade.quantity;
    cash_ -= trade.price * trade.quantity;
  } else {
      inventory_ -= trade.quantity;
      cash_ += trade.price * trade.quantity;
  }
  printf("[%s] Trade %llu | price: %.2f qty: %u | inv: %d cash: %.2f\n",
       name_.c_str(), trade.trade_id, trade.price, trade.quantity, inventory_, cash_);
}

void ExecutionAgent::schedule_order() {
    timer_.expires_after(std::chrono::milliseconds(interval_ms_));
    timer_.async_wait([this](std::error_code ec) {
        if (!ec) {
            send_order(buy_side_ ? 0 : 1, 0, target_price_, quantity_);
            schedule_order();
        }
    });
}

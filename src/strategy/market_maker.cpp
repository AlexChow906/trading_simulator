#include <cstdio>

#include "strategy/market_maker.h"

MarketMaker::MarketMaker(const std::string& host, uint16_t port, std::string name, double mid_price, double spread, uint64_t start_id)
    : Agent(host, port, name), mid_price_(mid_price), spread_(spread) {
      next_order_id_ = start_id;
    }

void MarketMaker::on_connected() {
  send_order(0, 0, mid_price_ - spread_ / 2, 10);
  send_order(1, 0, mid_price_ + spread_ / 2, 10);
}

void MarketMaker::on_trade(const TradeMessage& trade) {
  bool my_trade = (trade.buy_order_id >= 1000000 && trade.buy_order_id < 2000000) ||
                (trade.sell_order_id >= 1000000 && trade.sell_order_id < 2000000);
  if (!my_trade) return;

  bool was_buyer = (trade.buy_order_id >= 1000000);
  if (was_buyer) {
    inventory_ += trade.quantity;
    cash_ -= trade.price * trade.quantity;
  } else {
      inventory_ -= trade.quantity;
      cash_ += trade.price * trade.quantity;
  }
  printf("[MM] Trade %llu | price: %.2f qty: %u | inv: %d cash: %.2f\n",
       trade.trade_id, trade.price, trade.quantity, inventory_, cash_);
  on_connected();

  fills_received_++;
}

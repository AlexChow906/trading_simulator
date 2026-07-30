#pragma once

#include "strategy/agent.h"

class MarketMaker : public Agent {
  public:
    MarketMaker(const std::string& host, uint16_t port, double mid_price, double spread, uint64_t start_id);
    void on_connected() override;
    void on_trade(const TradeMessage& trade) override;

  private:
    double mid_price_;
    double spread_;
};

#pragma once

#include <string>

#include "strategy/agent.h"

class MarketMaker : public Agent {
  public:
    MarketMaker(const std::string& host, uint16_t port, std::string name, double mid_price, double spread, uint64_t start_id);
    void on_connected() override;
    void on_trade(const TradeMessage& trade) override;

  private:
    double mid_price_;
    double spread_;
};

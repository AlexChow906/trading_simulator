#pragma once

#include <deque>

#include "strategy/agent.h"

class MeanReversionAgent : public Agent {
  public:
    MeanReversionAgent(const std::string& host, uint16_t port, std::string name, uint32_t quantity, size_t window_size, double threshold, uint64_t start_id);
    void on_connected() override;
    void on_trade(const TradeMessage& trade) override;

  private:
    std::deque<double> trade_prices_;
    uint32_t quantity_;
    size_t window_size_;
    double threshold_;
    uint64_t start_id_;
};

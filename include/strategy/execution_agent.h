#pragma once

#include "strategy/agent.h"

class ExecutionAgent : public Agent {
  public:
    ExecutionAgent(const std::string& host, uint16_t port, std::string name, double target_price, uint32_t quantity, bool buy_side, uint32_t interval_ms, uint64_t start_id);
    void on_connected() override;
    void on_trade(const TradeMessage& trade) override;

  private:
    double target_price_;
    uint32_t quantity_;
    bool buy_side_;
    uint32_t interval_ms_;
    asio::steady_timer timer_;
    void schedule_order();
    uint64_t start_id_;
};

#pragma once

#include <string>
#include <cstdint>

#include "asio.hpp"
#include "gateway/message.h"

using asio::ip::tcp;

class Agent {
  public:
    Agent(const std::string& host, uint16_t port, std::string name);
    virtual ~Agent() = default;
    void run();
    void print_summary() const;
    void stop();

  protected:
    virtual void on_trade(const TradeMessage& trade) = 0;
    virtual void on_connected() = 0;
    void send_order(uint8_t side, uint8_t order_type, double price, uint32_t quantity);
    uint64_t next_order_id_{1};
    asio::io_context& io_context() { return io_context_; }
    int inventory_ = 0;
    double pnl_ = 0.0;
    double cash_ = 0.0;
    uint64_t orders_sent_{0};
    uint64_t fills_received_{0};
    std::string name_;

  private:
    void do_read();
    asio::io_context io_context_;
    tcp::socket socket_;
    TradeMessage read_buf_;
};

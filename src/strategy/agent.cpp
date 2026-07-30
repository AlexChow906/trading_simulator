#include "strategy/agent.h"
#include "gateway/message.h"

Agent::Agent(const std::string& host, uint16_t port)
    : socket_(io_context_) {
      tcp::resolver resolver(io_context_);
      auto endpoints = resolver.resolve(host, std::to_string(port));
      asio::connect(socket_, endpoints);
    }

void Agent::run() {
  on_connected();
  do_read();
  io_context_.run();
}

void Agent::do_read() {
  asio::async_read(socket_, asio::buffer(&read_buf_, sizeof(read_buf_)),
    [this](std::error_code ec, std::size_t) {
        if (!ec) {
            on_trade(read_buf_);
            do_read();
        }
    });
}

void Agent::send_order(uint8_t side, uint8_t order_type, double price, uint32_t quantity) {
  OrderMessage msg{next_order_id_++, side, order_type, price, quantity};
  asio::write(socket_, asio::buffer(&msg, sizeof(msg)));
}

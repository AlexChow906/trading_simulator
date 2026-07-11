#pragma once

#include "asio.hpp"
#include "matching_engine/engine_runner.h"
#include "gateway/message.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
  public:
    Session(tcp::socket socket, EngineRunner& runner);
    void start();

  private:
    void do_read();
    void do_write(TradeMessage msg);
    tcp::socket socket_;
    EngineRunner& runner_;
    OrderMessage read_buf_;
};

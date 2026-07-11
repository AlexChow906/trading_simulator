#pragma once

#include "asio.hpp"
#include "matching_engine/engine_runner.h"

using asio::ip::tcp;

class Gateway {
  public:
    Gateway(uint16_t port);
    void run();

  private:
    void do_accept();
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    EngineRunner runner_;
};

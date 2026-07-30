#pragma once

#include <vector>
#include <memory>

#include "asio.hpp"
#include "matching_engine/matching_engine.h"

using asio::ip::tcp;
class Session;

class Gateway {
  public:
    Gateway(uint16_t port);
    void run();

  private:
    void do_accept();
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    MatchingEngine engine_;
    std::vector<std::shared_ptr<Session>> sessions_;
};

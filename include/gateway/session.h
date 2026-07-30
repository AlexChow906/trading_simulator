#pragma once

#include "asio.hpp"
#include "matching_engine/matching_engine.h"
#include "gateway/message.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
  public:
    Session(tcp::socket socket, MatchingEngine& engine, std::vector<std::shared_ptr<Session>>& sessions);
    void start();
    void do_write(TradeMessage msg);

  private:
    void do_read();
    tcp::socket socket_;
    MatchingEngine& engine_;
    OrderMessage read_buf_;
    std::vector<std::shared_ptr<Session>>& sessions_;
};

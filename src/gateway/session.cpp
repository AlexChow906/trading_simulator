#include <cstdio>

#include "gateway/session.h"
#include "order_book/types.h"

Session::Session(tcp::socket socket, MatchingEngine& engine, std::vector<std::shared_ptr<Session>>& sessions)
    : socket_(std::move(socket)), engine_(engine), sessions_(sessions) {}

void Session::start() {
  do_read();
}

void Session::do_read() {
  asio::async_read(socket_, asio::buffer(&read_buf_, sizeof(read_buf_)),
    [this, self = shared_from_this()](std::error_code ec, std::size_t) {
        if (!ec) {
            Side side = static_cast<Side>(read_buf_.side);
            OrderType order_type = static_cast<OrderType>(read_buf_.order_type);
            Order order{read_buf_.order_id, side, order_type, read_buf_.price, read_buf_.quantity, 0};

            auto trades = engine_.process_order(order);
            for (const auto& trade : trades) {
                TradeMessage msg{trade.trade_id, trade.buy_order_id, trade.sell_order_id, trade.price, trade.quantity};
                for (auto& s : sessions_) {
                    s->do_write(msg);
                }
            }

            do_read();
        }
    });
}

void Session::do_write(TradeMessage msg) {
  auto buf = std::make_shared<TradeMessage>(msg);
  asio::async_write(socket_, asio::buffer(buf.get(), sizeof(TradeMessage)),
      [buf](std::error_code, std::size_t) {});
}

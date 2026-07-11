#include <memory>

#include "gateway/gateway.h"
#include "gateway/session.h"

Gateway::Gateway(uint16_t port)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {}

void Gateway::run() {
  runner_.start();
  do_accept();
  io_context_.run();
}

void Gateway::do_accept() {
  acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
    if (!ec) {
        auto session = std::make_shared<Session>(std::move(socket), runner_);
        session->start();
    }
    do_accept();
  });
}

#include "matching_engine/matching_engine.h"

std::vector<Trade> MatchingEngine::process_order(Order order) {
  std::vector<Trade> trades = book_.match_order(order, next_trade_id_);

  if (order.quantity > 0 && order.type == OrderType::Limit) {
    book_.add_order(order);
  }
  return trades;
}

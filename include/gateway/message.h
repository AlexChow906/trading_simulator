#pragma once

#include <cstdint>

#pragma pack(1)
struct OrderMessage {
  uint64_t order_id;
  uint8_t side;
  uint8_t order_type;
  double price;
  uint32_t quantity;
};

struct TradeMessage {
  uint64_t trade_id;
  uint64_t buy_order_id;
  uint64_t sell_order_id;
  double price;
  uint32_t quantity;
};
#pragma pack()

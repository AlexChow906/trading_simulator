#pragma once

#include <cstdint>
#include "types.h"

struct Order {
  uint64_t order_id;
  Side side;
  OrderType type;
  double price;
  uint32_t quantity;
  uint64_t timestamp;
};

#pragma once

#include <list>
#include "order.h"

struct PriceLevel{
  double price;
  std::list<Order> orders;
};

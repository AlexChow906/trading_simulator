#pragma once

#include "intrusive_list.h"
#include "order.h"

struct PriceLevel{
  double price;
  IntrusiveList orders;
};

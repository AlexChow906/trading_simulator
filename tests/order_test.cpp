#include <gtest/gtest.h>
#include "order_book/types.h"
#include "order_book/order.h"

TEST(OrderTest, ReadBackValues) {
    Order order1{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
    EXPECT_EQ(order1.order_id, 1);
    EXPECT_EQ(order1.side, Side::Buy);
    EXPECT_EQ(order1.type, OrderType::Limit);
    EXPECT_DOUBLE_EQ(order1.price, 100.5);
    EXPECT_EQ(order1.quantity, 10);
    EXPECT_EQ(order1.timestamp, 1000);

    Order order2{2, Side::Buy, OrderType::Market, 100.50, 10, 1000};
    EXPECT_EQ(order2.type, OrderType::Market);
}

TEST(OrderTest, CompareSides) {
    EXPECT_NE(Side::Buy, Side::Sell);
}

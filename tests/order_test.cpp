#include <gtest/gtest.h>
#include "order_book/types.h"
#include "order_book/order.h"
#include "order_book/order_book.h"

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

TEST(OrderBookTest, AddBuyOrderUpdatesBestBid) {
    Order order{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
    OrderBook book;

    book.add_order(order);
    EXPECT_TRUE(book.best_bid().has_value());
    EXPECT_DOUBLE_EQ(book.best_bid().value(), 100.50);
}

TEST(OrderBookTest, AddSellOrderUpdatesBestAsk) {
    Order order{1, Side::Sell, OrderType::Limit, 100.50, 10, 1000};
    OrderBook book;

    book.add_order(order);
    EXPECT_TRUE(book.best_ask().has_value());
    EXPECT_DOUBLE_EQ(book.best_ask().value(), 100.50);
}

TEST(OrderBookTest, EmptyBookReturnsNullopt) {
    OrderBook book;

    EXPECT_FALSE(book.best_bid().has_value());
    EXPECT_FALSE(book.best_ask().has_value());
}

TEST(OrderBookTest, BestBidReturnsHighestPrice) {
    Order order1{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
    Order order2{2, Side::Buy, OrderType::Limit, 110.50, 10, 1000};
    OrderBook book;

    book.add_order(order1);
    book.add_order(order2);
    EXPECT_TRUE(book.best_bid().has_value());
    EXPECT_DOUBLE_EQ(book.best_bid().value(), 110.50);
}

TEST(OrderBookTest, CancelOrderRemovesFromBook) {
    Order order{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
    OrderBook book;

    book.add_order(order);
    EXPECT_TRUE(book.best_bid().has_value());
    EXPECT_DOUBLE_EQ(book.best_bid().value(), 100.50);

    EXPECT_TRUE(book.cancel_order(1));
    EXPECT_FALSE(book.best_bid().has_value());
}

TEST(OrderBookTest, CancelNonexistentOrderReturnsFalse) {
    Order order{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
    OrderBook book;

    book.add_order(order);
    EXPECT_TRUE(book.best_bid().has_value());
    EXPECT_DOUBLE_EQ(book.best_bid().value(), 100.50);

    EXPECT_FALSE(book.cancel_order(999));
    EXPECT_TRUE(book.best_bid().has_value());
    EXPECT_DOUBLE_EQ(book.best_bid().value(), 100.50);
}

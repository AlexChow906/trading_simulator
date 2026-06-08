#include <gtest/gtest.h>
#include "order_book/types.h"
#include "order_book/order.h"
#include "matching_engine/matching_engine.h"

TEST(MatchingEngineTest, NoMatchRestsInBook) {
  Order order1{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  MatchingEngine engine;

  auto trades = engine.process_order(order1);

  EXPECT_EQ(trades.size(), 0);
  EXPECT_DOUBLE_EQ(engine.best_bid().value(), order1.price);
}

TEST(MatchingEngineTest, FullFill) {
  Order sell_order{1, Side::Sell, OrderType::Limit, 100.50, 10, 1000};
  Order buy_order{2, Side::Buy, OrderType::Limit, 100.50, 10, 1001};
  MatchingEngine engine;

  engine.process_order(sell_order);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.5);
  EXPECT_EQ(trades[0].quantity, 10);
}

TEST(MatchingEngineTest, PartialFillIncomingLarger) {
  Order sell_order{1, Side::Sell, OrderType::Limit, 100.50, 10, 1000};
  Order buy_order{2, Side::Buy, OrderType::Limit, 100.50, 30, 1001};
  MatchingEngine engine;

  engine.process_order(sell_order);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.50);
  EXPECT_EQ(trades[0].quantity, 10);
  EXPECT_TRUE(engine.best_bid().has_value());
  EXPECT_DOUBLE_EQ(engine.best_bid().value(), 100.50);
}

TEST(MatchingEngineTest, PartialFillRestingLarger) {
  Order sell_order{1, Side::Sell, OrderType::Limit, 100.50, 30, 1000};
  Order buy_order{2, Side::Buy, OrderType::Limit, 100.50, 10, 1001};
  MatchingEngine engine;

  engine.process_order(sell_order);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.50);
  EXPECT_EQ(trades[0].quantity, 10);
  EXPECT_TRUE(engine.best_ask().has_value());
  EXPECT_DOUBLE_EQ(engine.best_ask().value(), 100.50);
}

TEST(MatchingEngineTest, WalkTheBook) {
  Order sell_order1{1, Side::Sell, OrderType::Limit, 100.00, 10, 1000};
  Order sell_order2{2, Side::Sell, OrderType::Limit, 102.00, 30, 1001};
  Order buy_order{3, Side::Buy, OrderType::Limit, 105.00, 40, 1002};
  MatchingEngine engine;

  engine.process_order(sell_order1);
  engine.process_order(sell_order2);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 2);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.00);
  EXPECT_DOUBLE_EQ(trades[1].price, 102.00);
  EXPECT_EQ(trades[0].quantity, 10);
  EXPECT_EQ(trades[1].quantity, 30);
  EXPECT_FALSE(engine.best_ask().has_value());
  EXPECT_FALSE(engine.best_bid().has_value());
}

TEST(MatchingEngineTest, TradeExecutesAtRestingPrice) {
  Order sell_order{1, Side::Sell, OrderType::Limit, 100.00, 30, 1000};
  Order buy_order{2, Side::Buy, OrderType::Limit, 105.00, 30, 1001};
  MatchingEngine engine;

  engine.process_order(sell_order);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.00);
  EXPECT_EQ(trades[0].quantity, 30);
  EXPECT_FALSE(engine.best_ask().has_value());
  EXPECT_FALSE(engine.best_bid().has_value());
}

TEST(MatchingEngineTest, MatchesMarketOrders) {
  Order sell_order{1, Side::Sell, OrderType::Limit, 100.00, 30, 1000};
  Order buy_order{2, Side::Buy, OrderType::Market, 105.00, 40, 1001};
  MatchingEngine engine;

  engine.process_order(sell_order);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.00);
  EXPECT_EQ(trades[0].quantity, 30);
  EXPECT_FALSE(engine.best_ask().has_value());
  EXPECT_FALSE(engine.best_bid().has_value());
}

TEST(MatchingEngineTest, WalkTheBookRespectsPriceTimePriority) {
  Order sell_order1{1, Side::Sell, OrderType::Limit, 100.00, 10, 1000};
  Order sell_order2{2, Side::Sell, OrderType::Limit, 100.00, 10, 1002};
  Order buy_order{3, Side::Buy, OrderType::Limit, 105.00, 10, 1003};
  MatchingEngine engine;

  engine.process_order(sell_order1);
  engine.process_order(sell_order2);
  auto trades = engine.process_order(buy_order);

  EXPECT_EQ(trades.size(), 1);
  EXPECT_DOUBLE_EQ(trades[0].price, 100.00);
  EXPECT_EQ(trades[0].quantity, 10);
  EXPECT_EQ(trades[0].sell_order_id, 1);
  EXPECT_FALSE(engine.best_bid().has_value());
  EXPECT_TRUE(engine.best_ask().has_value());
  EXPECT_DOUBLE_EQ(engine.best_ask().value(), 100.00);
}

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "matching_engine/engine_runner.h"

TEST(EngineRunnerTest, SubmitAndProcess) {
  Order order1{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  EngineRunner engine_runner;

  engine_runner.start();
  engine_runner.submit(order1);
  engine_runner.stop();

  EXPECT_EQ(engine_runner.orders_processed(), 1);
}

TEST(EngineRunnerTest, MultipleOrders) {
  Order order1{1, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  Order order2{2, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  Order order3{3, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  Order order4{4, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  Order order5{5, Side::Buy, OrderType::Limit, 100.50, 10, 1000};
  EngineRunner engine_runner;

  engine_runner.start();
  engine_runner.submit(order1);
  engine_runner.submit(order2);
  engine_runner.submit(order3);
  engine_runner.submit(order4);
  engine_runner.submit(order5);
  engine_runner.stop();

  EXPECT_EQ(engine_runner.orders_processed(), 5);
}

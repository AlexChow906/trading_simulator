#include <gtest/gtest.h>

#include "concurrency/thread_safe_queue.h"

TEST(ThreadSafeQueueTest, FIFOOrdering) {
  ThreadSafeQueue<int> queue;

  queue.push(10);
  queue.push(20);
  queue.push(30);

  EXPECT_EQ(queue.pop(), 10);
  EXPECT_EQ(queue.pop(), 20);
  EXPECT_EQ(queue.pop(), 30);
}

TEST(ThreadSafeQueueTest, StopUnblocksEmptyQueue) {
  ThreadSafeQueue<int> queue;

  queue.stop();
  auto result = queue.pop();

  EXPECT_FALSE(result.has_value());
}

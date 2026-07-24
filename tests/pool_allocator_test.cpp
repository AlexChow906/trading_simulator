#include <gtest/gtest.h>

#include "concurrency/pool_allocator.h"

TEST(PoolAllocatorTest, AllocateAndDeallocate) {
  PoolAllocator<int, 4> pool_allocator;

  int* ptr = pool_allocator.allocate();
  *ptr = 42;
  EXPECT_EQ(*ptr, 42);
  pool_allocator.deallocate(ptr);
}

TEST(PoolAllocatorTest, PoolExhaustion) {
  PoolAllocator<int, 2> pool_allocator;

  int* ptr1 = pool_allocator.allocate();
  *ptr1 = 42;
  EXPECT_NE(ptr1, nullptr);

  int* ptr2 = pool_allocator.allocate();
  *ptr2 = 42;
  EXPECT_NE(ptr2, nullptr);

  int* ptr3 = pool_allocator.allocate();
  EXPECT_EQ(ptr3, nullptr);

  pool_allocator.deallocate(ptr2);

  int* ptr4 = pool_allocator.allocate();
  *ptr4 = 42;
  EXPECT_NE(ptr4, nullptr);
}

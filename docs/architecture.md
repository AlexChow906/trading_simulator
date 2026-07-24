# Architecture

## Data Flow

```
  TCP Clients
      │
      ▼
  ┌─────────┐
  │ Gateway  │  Boost.Asio async I/O
  │          │  Accepts connections, deserializes OrderMessages
  └────┬─────┘
       │ submit(Order)
       ▼
  ┌──────────────┐
  │ EngineRunner  │  Orchestrates threads and queues
  │               │
  │  ┌──────────┐ │
  │  │ in_queue_ │◄── ThreadSafeQueue<Order>
  │  └────┬─────┘ │
  │       │ pop()  │
  │       ▼        │
  │  ┌───────────┐ │
  │  │ Matching   │ │  Single-threaded matching core
  │  │ Engine     │ │  Owns OrderBook (one per instrument)
  │  │            │ │
  │  │ OrderBook  │ │  std::map<price, PriceLevel>
  │  │  ├ bids_   │ │  IntrusiveList of pool-allocated Orders
  │  │  ├ asks_   │ │  PoolAllocator<Order> — zero-alloc hot path
  │  │  └ lookup_ │ │  unordered_map<id, Order*> for O(1) cancel
  │  └────┬──────┘ │
  │       │ trades  │
  │       ▼        │
  │ ┌────────────┐ │
  │ │trades_queue_│──► ThreadSafeQueue<Trade>
  │ └────────────┘ │
  └────────────────┘
       │ pop()
       ▼
  Market Data Thread
  (prints / broadcasts trades)
```

## Threading Model

```
  Main Thread          Matching Thread         Market Data Thread
  ───────────          ───────────────         ──────────────────
  Gateway::run()
  io_context_.run()
       │
       │  async_accept
       │  async_read ──► in_queue_.push()
       │                      │
       │                      ▼
       │               matching_loop()
       │               pop() ──► process_order()
       │                              │
       │                         trades_queue_.push()
       │                              │
       │                              ▼
       │                        market_data_loop()
       │                        pop() ──► printf()
       │
  stop():
  1. in_queue_.stop()
  2. join matching_thread_     ◄── drains all pending orders
  3. trades_queue_.stop()
  4. join market_data_thread_  ◄── drains all pending trades
```

## Key Design Decisions

- **One matching engine per instrument** — single-threaded matching core avoids locks on the order book. All concurrency lives in the queues.
- **Pool allocator** — pre-allocated slab of Order slots. Allocate/deallocate are O(1) free-list operations. Eliminated 35% of p99.9 tail latency.
- **Intrusive linked list** — prev/next pointers inside Order struct. No per-node allocation, better cache locality than std::list.
- **Ordered shutdown** — drain in-queue before trades-queue ensures every submitted order produces its trades before the engine stops.

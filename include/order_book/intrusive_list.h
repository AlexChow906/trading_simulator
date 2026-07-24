#pragma once

#include <cstddef>

#include "order.h"

class IntrusiveList {
  public:
    void push_back(Order* order) {
      if (size_ == 0) {
        head_ = order;
        tail_ = order;
      } else {
        tail_->next = order;
        order->prev = tail_;
        tail_ = order;
      }

      size_++;
    }

    void remove(Order* order) {
      if (order->prev != nullptr) {
        order->prev->next = order->next;
      } else {
        head_ = order->next;
      }

      if (order->next != nullptr) {
        order->next->prev = order->prev;
      } else {
        tail_ = order->prev;
      }

      order->prev = nullptr;
      order->next = nullptr;
      size_--;
    }

    Order* front() {
      return head_;
    }

    bool empty() {
      return size_ == 0;
    }

    std::size_t size() {
      return size_;
    }

  private:
    Order* head_ = nullptr;
    Order* tail_ = nullptr;
    std::size_t size_ = 0;
};

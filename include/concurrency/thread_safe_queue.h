#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class ThreadSafeQueue{
  public:
    void push(T item) {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.push(std::move(item));
      condition_.notify_one();
    }

    std::optional<T> pop() {
      std::unique_lock<std::mutex> lock(mutex_);
      condition_.wait(lock, [this] { return !queue_.empty() || stopped_; });

      if (queue_.empty()) {
        return std::nullopt;
      }

      auto item = std::move(queue_.front());
      queue_.pop();
      return item;
    }

    void stop() {
      std::lock_guard<std::mutex> lock(mutex_);
      stopped_ = true;
      condition_.notify_all();
    }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stopped_ = false;
};

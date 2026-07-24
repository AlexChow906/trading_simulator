#pragma once

#include <vector>
#include <mutex>
#include <cstddef>

template <typename T, std::size_t N>
class PoolAllocator {
  private:
    union Slot {
      Slot* next;
      alignas(T) unsigned char storage[sizeof(T)];
    };

  public:
    PoolAllocator() : slots_(N) {
      for (std::size_t i = 0; i < N - 1; ++i) {
        slots_[i].next = &slots_[i + 1];
      }
      slots_[N - 1].next = nullptr;
      free_list_ = &slots_[0];
    }

    T* allocate() {
      std::lock_guard<std::mutex> lock(mutex_);
      if (free_list_ == nullptr) {
        return nullptr;
      }
      Slot* slot = free_list_;
      free_list_ = slot->next;
      return reinterpret_cast<T*>(slot);
    }

    void deallocate(T* ptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      Slot* slot = reinterpret_cast<Slot*>(ptr);
      slot->next = free_list_;
      free_list_ = slot;
    }

  private:
    std::vector<Slot> slots_;
    Slot* free_list_;
    std::mutex mutex_;
};

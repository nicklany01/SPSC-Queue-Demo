#pragma once
#include <atomic>
#include <bit>
#include <cstddef>
#include <new>
#include <type_traits>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_destructive_interference_size;
#else
constexpr size_t hardware_destructive_interference_size = 64;
#endif

template <typename T, size_t RequestedCapacity>
  requires std::is_trivially_copyable_v<T> // required to use an array since no
                                           // args are accepted in constructor
class SpscQueue {
public:
  static constexpr size_t Capacity = std::bit_ceil(RequestedCapacity);
  static constexpr size_t Mask = Capacity - 1; // since Capacity is power of 2

private:
  alignas(hardware_destructive_interference_size) T buffer_[Capacity];
  alignas(hardware_destructive_interference_size) std::atomic<size_t> start_ =
      0;
  alignas(hardware_destructive_interference_size) std::atomic<size_t> end_ = 0;

public:
  SpscQueue() {}

  bool push(const T &item) {
    size_t start = start_.load(
        std::memory_order_acquire); // uses acquire so a buffer read is
                                    // guaranteed to finish before the write
    size_t end = end_.load(std::memory_order_relaxed);
    if (end - start >= Capacity) {
      return false;
    }

    buffer_[end & Mask] = item;
    end_.store(end + 1, std::memory_order_release);
    return true;
  }

  bool pop(T &item) {
    size_t start = start_.load(std::memory_order_relaxed);
    size_t end = end_.load(
        std::memory_order_acquire); // uses acquire so a buffer write is
                                    // guaranteed to finish before the read
    if (start == end) {
      return false;
    }

    item = buffer_[start & Mask];
    start_.store(start + 1, std::memory_order_release);
    return true;
  }

  [[nodiscard]] bool empty() const {
    return start_.load(std::memory_order_relaxed) ==
           end_.load(std::memory_order_relaxed);
  }

  [[nodiscard]] size_t size() const {
    return end_.load(std::memory_order_relaxed) -
           start_.load(std::memory_order_relaxed);
  }

  [[nodiscard]] size_t capacity() const { return Capacity; }
};

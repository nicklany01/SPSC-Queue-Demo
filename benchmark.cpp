#include "spsc_queue.hpp"
#include <benchmark/benchmark.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <sched.h>

constexpr size_t QUEUE_CAPACITY = 1024;

template <typename T, size_t Capacity> class MutexQueue {
  T buffer_[Capacity];
  size_t head_ = 0;
  size_t tail_ = 0;
  size_t count_ = 0;
  std::mutex mtx_;

public:
  bool push(const T &item) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (count_ == Capacity)
      return false;
    buffer_[tail_] = item;
    tail_ = (tail_ + 1) % Capacity;
    count_++;
    return true;
  }

  bool pop(T &item) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (count_ == 0)
      return false;
    item = buffer_[head_];
    head_ = (head_ + 1) % Capacity;
    count_--;
    return true;
  }
};

void pin_thread_to_core(int core_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// Queues dynamically managed per benchmark run
static std::unique_ptr<SpscQueue<int, QUEUE_CAPACITY>> g_spsc;
static std::unique_ptr<MutexQueue<int, QUEUE_CAPACITY>> g_mutex;

static void BM_SpscQueue(benchmark::State &state) {
  if (state.thread_index() == 0) {
    pin_thread_to_core(2); // Physical Core 2
    g_spsc = std::make_unique<SpscQueue<int, QUEUE_CAPACITY>>();
    int i = 0;
    for (auto _ : state) {
      while (!g_spsc->push(i)) {
      }
      i++;
    }
    state.SetItemsProcessed(state.iterations());
  } else if (state.thread_index() == 1) {
    pin_thread_to_core(3); // Physical Core 3
    int val = 0;
    for (auto _ : state) {
      while (!g_spsc || !g_spsc->pop(val)) {
      }
      benchmark::DoNotOptimize(val);
    }
  }
}

static void BM_MutexQueue(benchmark::State &state) {
  if (state.thread_index() == 0) {
    pin_thread_to_core(2); // Physical Core 2
    g_mutex = std::make_unique<MutexQueue<int, QUEUE_CAPACITY>>();
    int i = 0;
    for (auto _ : state) {
      while (!g_mutex->push(i)) {
      }
      i++;
    }
    state.SetItemsProcessed(state.iterations());
  } else if (state.thread_index() == 1) {
    pin_thread_to_core(3); // Physical Core 3
    int val = 0;
    for (auto _ : state) {
      while (!g_mutex || !g_mutex->pop(val)) {
      }
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_SpscQueue)->Threads(2)->UseRealTime();
BENCHMARK(BM_MutexQueue)->Threads(2)->UseRealTime();

BENCHMARK_MAIN();

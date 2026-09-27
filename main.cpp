#include "spsc_queue.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

void test_initial_state() {
  SpscQueue<int, 4> q;
  int val = 0;

  assert(q.empty());
  assert(q.size() == 0);
  assert(q.capacity() == 4);
  assert(!q.pop(val)); // Pop on empty must fail
  std::cout << "[PASS] Initial state\n";
}

void test_basic_fifo() {
  SpscQueue<int, 4> q;
  int val = 0;

  assert(q.push(10));
  assert(q.push(20));
  assert(q.size() == 2);
  assert(!q.empty());

  assert(q.pop(val) && val == 10);
  assert(q.pop(val) && val == 20);
  assert(q.empty());
  assert(q.size() == 0);
  std::cout << "[PASS] Basic FIFO ordering\n";
}

void test_full_capacity_rejection() {
  constexpr size_t CAP = 4;
  SpscQueue<int, CAP> q;
  int val = 0;

  // Fill to capacity
  for (size_t i = 0; i < CAP; ++i) {
    assert(q.push(static_cast<int>(i)));
  }

  assert(q.size() == CAP);

  // Push past capacity must fail and not modify state
  assert(!q.push(999));
  assert(q.size() == CAP);

  // Ensure elements remained intact
  for (size_t i = 0; i < CAP; ++i) {
    assert(q.pop(val) && val == static_cast<int>(i));
  }
  assert(q.empty());
  std::cout << "[PASS] Full capacity boundary rejection\n";
}

void test_circular_wraparound() {
  constexpr size_t CAP = 4;
  SpscQueue<int, CAP> q;
  int val = 0;

  // Push and pop 100 times to force indices past Capacity repeatedly
  for (int i = 0; i < 100; ++i) {
    assert(q.push(i));
    assert(q.push(i + 1));
    assert(q.pop(val) && val == i);
    assert(q.pop(val) && val == i + 1);
    assert(q.empty());
  }

  std::cout << "[PASS] Circular wraparound\n";
}

void test_custom_data_type() {
  struct Order {
    uint32_t id;
    double price;
    uint32_t qty;
  };

  SpscQueue<Order, 8> q;
  Order in{101, 450.25, 50};
  Order out{};

  assert(q.push(in));
  assert(q.pop(out));
  assert(out.id == 101);
  assert(out.price == 450.25);
  assert(out.qty == 50);

  std::cout << "[PASS] Custom struct data type\n";
}

int main() {
  test_initial_state();
  test_basic_fifo();
  test_full_capacity_rejection();
  test_circular_wraparound();
  test_custom_data_type();

  std::cout << "\nAll test suites passed successfully.\n";
  return 0;
}

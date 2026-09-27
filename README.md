# Cache-Aligned Lock-Free SPSC Queue

This project is a high-performance, lock-free Single-Producer Single-Consumer
(SPSC) ring buffer written in C++20.

## Project Goals

- Provide a lock-free synchronization mechanism without OS-level context
  switches.
- Ensure deterministic performance for hot-path systems.
- Serve as a foundation for a zero-copy market data parser.

## Technical Specifications

- **Time Complexity:** O(1) for push and pop operations.
- **Space Complexity:** O(N) where N is the fixed capacity.
- **Concurrency:** Thread-safe for one producer and one consumer operating
  simultaneously.
- **Memory Optimizations:**
  - Cache-line aligned to prevent false sharing between CPU cores.
  - Zero heap allocations during the operational lifecycle.
  - Optimized indexing mechanisms for wrapping bounds.
- **Type Safety:** Enforces trivial copyability for queue elements.

## Build and Run

To build the project and run the provided test harness:

```bash
make
./main
```

## Benchmarks

Run the benchmark suite and `perf` profile via:

```bash
make run-benchmark
```

### Results

Comparing the lock-free `SpscQueue` vs a standard `std::mutex` approach using two threads pinned to separate physical cores:

```text
Benchmark                                 Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------
BM_SpscQueue/real_time/threads:2       12.3 ns         12.3 ns     54106360 items_per_second=40.5059M/s
BM_MutexQueue/real_time/threads:2       51.9 ns         51.8 ns     13463206 items_per_second=9.63454M/s
```

Perf stat results for `SpscQueue`:

```text
        63,361,224      L1-dcache-load-misses                                                 
        53,301,150      cache-misses                                                          
                43      context-switches 
```

Perf stat results for `MutexQueue`:

```text
        65,036,951      L1-dcache-load-misses                                                 
         8,639,573      cache-misses                                                          
               130      context-switches 
```

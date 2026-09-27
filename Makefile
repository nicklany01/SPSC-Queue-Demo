CXX = g++
CXXFLAGS_BASE = -std=c++20 -Wall -Wextra -Wpedantic -pthread

RELEASE_FLAGS = -O3 -march=native -DNDEBUG

TSAN_FLAGS = -O1 -g -fsanitize=thread

ASAN_FLAGS = -O1 -g -fsanitize=address,undefined

.PHONY: all release tsan asan benchmark run-benchmark clean

all: release tsan asan

release:
	$(CXX) $(CXXFLAGS_BASE) $(RELEASE_FLAGS) -o build/main main.cpp

tsan:
	$(CXX) $(CXXFLAGS_BASE) $(TSAN_FLAGS) -o build/main_tsan main.cpp

asan:
	$(CXX) $(CXXFLAGS_BASE) $(ASAN_FLAGS) -o build/main_asan main.cpp

benchmark:
	$(CXX) $(CXXFLAGS_BASE) $(RELEASE_FLAGS) -o build/benchmark benchmark.cpp -lbenchmark -lbenchmark_main

run-benchmark: benchmark
	sudo perf stat -e L1-dcache-load-misses,cache-misses,context-switches ./build/benchmark --benchmark_filter=BM_SpscQueue
	sudo perf stat -e L1-dcache-load-misses,cache-misses,context-switches ./build/benchmark --benchmark_filter=BM_MutexQueue

clean:
	rm -f build/main build/main_tsan build/main_asan build/benchmark

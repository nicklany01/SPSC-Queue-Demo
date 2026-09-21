CXX = g++
CXXFLAGS_BASE = -std=c++20 -Wall -Wextra -Wpedantic -pthread

RELEASE_FLAGS = -O3 -march=native -DNDEBUG

TSAN_FLAGS = -O1 -g -fsanitize=thread

ASAN_FLAGS = -O1 -g -fsanitize=address,undefined

.PHONY: all release tsan asan clean

all: release tsan asan

release:
	$(CXX) $(CXXFLAGS_BASE) $(RELEASE_FLAGS) -o build/main main.cpp

tsan:
	$(CXX) $(CXXFLAGS_BASE) $(TSAN_FLAGS) -o build/main_tsan main.cpp

asan:
	$(CXX) $(CXXFLAGS_BASE) $(ASAN_FLAGS) -o build/main_asan main.cpp

clean:
	rm -f main main_tsan main_asan

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc -Itests
TEST_BINARY := build/test_traffic_controller

SOURCES := src/TrafficController.cpp tests/test_traffic_controller.cpp

.PHONY: test clean

test:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TEST_BINARY)
	./$(TEST_BINARY)

clean:
	rm -rf build

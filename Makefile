CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude -Isrc
SRC := $(wildcard src/*.cpp)
BUILD_DIR := build
BIN := $(BUILD_DIR)/bin_prog

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean



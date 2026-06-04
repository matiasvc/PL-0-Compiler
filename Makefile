CXX := clang++

CXXFLAGS := -std=c++17 -g -Wall -Wextra
LIBS := -lbsd

BUILD := build
OBJS := $(BUILD)/main.o $(BUILD)/lexer.o $(BUILD)/util.o $(BUILD)/global.o

$(BUILD)/pl0c: $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $(BUILD)/pl0c

$(BUILD)/main.o: main.cc global.h lexer.h util.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -c main.cc -o $(BUILD)/main.o

$(BUILD)/lexer.o: lexer.cc lexer.h global.h util.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -c lexer.cc -o $(BUILD)/lexer.o

$(BUILD)/util.o: util.cc util.h global.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -c util.cc -o $(BUILD)/util.o

$(BUILD)/global.o: global.cc global.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -c global.cc -o $(BUILD)/global.o

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -rf $(BUILD)

CXX := clang++

# -MMD -MP makes the compiler emit .d dependency files so header changes
# trigger rebuilds without hand-maintained dependency lists.
CXXFLAGS := -std=c++17 -g -Wall -Wextra -MMD -MP
LIBS := -lbsd

BUILD := build
SRCS := $(wildcard *.cc)
OBJS := $(SRCS:%.cc=$(BUILD)/%.o)

$(BUILD)/pl0c: $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $@

$(BUILD)/%.o: %.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

-include $(OBJS:.o=.d)

.PHONY: test clean

test: $(BUILD)/pl0c
	cd tests && ./test.sh

clean:
	rm -rf $(BUILD)

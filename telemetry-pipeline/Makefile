CC := gcc
CFLAGS := -Wall -Wextra -O2 -Iinclude
LDFLAGS := -pthread -lrt

BUILD := build
BIN := $(BUILD)/bin

COMMON_SRC := \
	src/common/ipc.c \
	src/common/ringbuf.c \
	src/common/sync.c \
	src/common/queue.c \
	src/common/stats.c


PRODUCER_SRC := src/producer/producer.c
AGGREGATOR_SRC := \
    src/aggregator/aggregator.c \
    src/aggregator/reader.c \
    src/aggregator/worker.c \
	src/aggregator/stats_thread.c

COMMON_OBJ := $(COMMON_SRC:src/%.c=$(BUILD)/%.o)
PRODUCER_OBJ := $(PRODUCER_SRC:src/%.c=$(BUILD)/%.o)
AGGREGATOR_OBJ := $(AGGREGATOR_SRC:src/%.c=$(BUILD)/%.o)

all: $(BIN)/producer $(BIN)/aggregator

# compile rule: create obj directory automatically
$(BUILD)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# link outputs go to build/bin/
$(BIN)/producer: $(COMMON_OBJ) $(PRODUCER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BIN)/aggregator: $(COMMON_OBJ) $(AGGREGATOR_OBJ)
	@mkdir -p $(BIN)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD)

.PHONY: all clean
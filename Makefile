CC        := clang
CFLAGS    := -std=c11 \
						 -Wall -Wextra -Wpedantic \
						 -O3 \
						 -flto \
						 -fomit-frame-pointer \
						 -march=native -mtune=native

SRC_DIR   := ./src
BUILD_DIR := ./build

SRCS      := $(wildcard $(SRC_DIR)/*.c)
OBJS      := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET    := $(BUILD_DIR)/polaris

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean

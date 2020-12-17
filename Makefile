TARGET := prog
CC := gcc

CFLAGS := -std=c99
LDFLAGS :=

SRC_DIR := src
BUILD_DIR := build

ifeq ($(DEBUG),1)
	CFLAGS += -g -ggdb
else
	CFLAGS += -O3
endif


SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

debug:
	@echo $(SRCS)
	@echo $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	
.PHONY: all clean debug


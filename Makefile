TARGET := prog
CC := gcc

CFLAGS := -std=c99 -Wall -pedantic
LDFLAGS :=
PACKAGES := sdl2

CFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

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
	@echo CFLAGS=$(CFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo SRCS=$(SRCS)
	@echo OBJS=$(OBJS)

$(TARGET): $(OBJS)
	$(CC) $< -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	
.PHONY: all clean debug


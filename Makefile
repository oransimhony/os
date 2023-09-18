CFLAGS = -Wall -Wextra -Wpedantic -std=c99
BUILD_DIR = build
TARGET = $(BUILD_DIR)/os

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -ggdb -O0
else
	CFLAGS += -DNDEBUG -O2
endif


.PHONY: all
all: test

.PHONY: test
test: $(TARGET)
$(TARGET): test.c os_sb.h os_sv.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@
	$@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

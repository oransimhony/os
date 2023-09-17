CFLAGS = -Wall -Wextra -Wpedantic -std=c99
BUILD_DIR = build
TARGET = $(BUILD_DIR)/sv

ifdef $(DEBUG)
	CFLAGS += -ggdb -O0
else
	CFLAGS += -O2
endif


.PHONY: all
all: test

.PHONY: test
test: $(TARGET)
$(TARGET): sv.c test.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	$@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

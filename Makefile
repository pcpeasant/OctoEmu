EXEC := Chippy
SRC_DIR := ./src
BUILD_DIR := ./build

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(BUILD_DIR)/$(SRCS:.c=.o)

LDFLAGS := -lSDL2 -lSDL2_mixer

$(BUILD_DIR)/$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
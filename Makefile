CC = gcc
CFLAGS = -Wall -Wextra -O3 -I./include
LDFLAGS = -lm

SRC_DIR = src
BUILD_DIR = build
OUTPUT_DIR = output

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET = lga.exe

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET) $(OUTPUT_DIR)/*.ppm grid.bin result.txt

.PHONY: all clean directories
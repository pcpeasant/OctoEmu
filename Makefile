SRC_DIR = src
BUILD_DIR = build

Chippy: main.o
	cc $(BUILD_DIR)/main.o -o $(BUILD_DIR)/Chippy -lSDL2

main.o: $(SRC_DIR)/main.c
	cc -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o 
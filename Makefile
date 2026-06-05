BUILD_DIR := build

.PHONY: all configure clean run

all: configure
	$(MAKE) -C $(BUILD_DIR)

configure:
	cmake -S . -B $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(BUILD_DIR)/sim

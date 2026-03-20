ASM=nasm

SRC_DIR=src
BUILD_DIR=build

BOOT_BIN=$(BUILD_DIR)/boot.bin
GAME_BIN=$(BUILD_DIR)/game.bin
BOOT_IMG=$(BUILD_DIR)/boot.img

all: $(BOOT_IMG)

$(BOOT_IMG): $(BOOT_BIN) $(GAME_BIN)
	cat $(BOOT_BIN) $(GAME_BIN) > $(BOOT_IMG)
	truncate -s 1440k $(BOOT_IMG)

$(BOOT_BIN): $(SRC_DIR)/boot.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) $(SRC_DIR)/boot.asm -f bin -o $(BOOT_BIN)

$(GAME_BIN): $(SRC_DIR)/myname.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) $(SRC_DIR)/myname.asm -f bin -o $(GAME_BIN)

clean:
	rm -rf $(BUILD_DIR)
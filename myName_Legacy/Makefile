ASM       = nasm
SRC_DIR   = src
BUILD_DIR = build
BOOT_BIN  = $(BUILD_DIR)/boot.bin
GAME_BIN  = $(BUILD_DIR)/game.bin
BOOT_IMG  = $(BUILD_DIR)/boot.img

IMG_SIZE_MB = 2

all: $(BOOT_IMG)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BOOT_BIN): $(SRC_DIR)/boot.asm | $(BUILD_DIR)
	@echo "[1/2] Assembling bootloader..."
	@$(ASM) $< -f bin -o $@

$(GAME_BIN): $(SRC_DIR)/myname.asm | $(BUILD_DIR)
	@echo "[2/2] Assembling game..."
	@$(ASM) $< -f bin -o $@

$(BOOT_IMG): $(BOOT_BIN) $(GAME_BIN)
	@echo "[ * ] Creating disk image..."
	@dd if=/dev/zero of=$(BOOT_IMG) bs=1M count=$(IMG_SIZE_MB) 2>/dev/null
	@dd if=$(BOOT_BIN) of=$(BOOT_IMG) bs=512 conv=notrunc 2>/dev/null
	@dd if=$(GAME_BIN) of=$(BOOT_IMG) bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "[ * ] Image ready: $(BOOT_IMG)"

run: $(BOOT_IMG)
	@echo "[ * ] Launching QEMU (legacy BIOS)..."
	@qemu-system-x86_64 \
		-drive format=raw,file=$(BOOT_IMG) \
		-net none

clean:
	@echo "[ * ] Cleaning..."
	@rm -rf $(BUILD_DIR)

.PHONY: all run cleanc
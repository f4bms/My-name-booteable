ARCH        = x86_64
BOOT_EFI    = BOOTX64.EFI
GAME_EFI    = GAME.EFI
IMAGE       = build/bootloader.img
IMG_SIZE_MB = 64
HEADERS     = ./src/efi_headers
SRC         = ./src
BUILD       = ./build

CC = clang
LD = lld-link

CFLAGS = \
	-target x86_64-unknown-windows \
	-ffreestanding \
	-fshort-wchar \
	-mno-red-zone \
	-fno-stack-protector \
	-fno-stack-check \
	-I$(HEADERS) \
	-I$(HEADERS)/protocol \
	-Wall -Wextra

LDFLAGS = \
	-subsystem:efi_application \
	-nodefaultlib

all: $(BUILD)/$(BOOT_EFI) $(BUILD)/$(GAME_EFI)

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/$(BOOT_EFI): $(SRC)/bootloader.c | $(BUILD)
	@echo "[1/2] Compiling bootloader..."
	@$(CC) $(CFLAGS) -c $< -o $(BUILD)/bootloader.o
	@$(LD) $(LDFLAGS) -entry:efi_main $(BUILD)/bootloader.o -out:$@

$(BUILD)/$(GAME_EFI): $(SRC)/game.c | $(BUILD)
	@echo "[2/2] Compiling game..."
	@$(CC) $(CFLAGS) -c $< -o $(BUILD)/game.o
	@$(LD) $(LDFLAGS) -entry:efi_main $(BUILD)/game.o -out:$@

efi-dir: all
	@mkdir -p $(BUILD)/EFI/BOOT
	@cp $(BUILD)/$(BOOT_EFI) $(BUILD)/EFI/BOOT/BOOTX64.EFI
	@cp $(BUILD)/$(GAME_EFI) $(BUILD)/EFI/BOOT/GAME.EFI

image: efi-dir
	@echo "[ * ] Creating disk image..."
	@dd if=/dev/zero of=$(IMAGE) bs=1M count=$(IMG_SIZE_MB) 2>/dev/null
	@parted -s $(IMAGE) mklabel gpt
	@parted -s $(IMAGE) mkpart ESP fat32 1MiB 100%
	@parted -s $(IMAGE) set 1 esp on
	@LOOP=$$(sudo losetup -f --show -P $(IMAGE)); \
	sudo mkfs.fat -F32 $${LOOP}p1 > /dev/null; \
	sudo mkdir -p /mnt/efi_tmp; \
	sudo mount $${LOOP}p1 /mnt/efi_tmp; \
	sudo mkdir -p /mnt/efi_tmp/EFI/BOOT; \
	sudo cp $(BUILD)/$(BOOT_EFI) /mnt/efi_tmp/EFI/BOOT/BOOTX64.EFI; \
	sudo cp $(BUILD)/$(GAME_EFI) /mnt/efi_tmp/EFI/BOOT/GAME.EFI; \
	sudo umount /mnt/efi_tmp; \
	sudo losetup -d $$LOOP
	@echo "[ * ] Image ready: $(IMAGE)"

run: image
	@echo "[ * ] Launching QEMU..."
	@qemu-system-x86_64 \
		-bios /usr/share/ovmf/x64/OVMF.4m.fd \
		-drive format=raw,file=$(IMAGE) \
		-net none

flash: image
	@echo "[ * ] Available devices:"
	@lsblk -d -o NAME,SIZE,MODEL
	@read -p "    Target device (e.g. sdb): " DEV; \
	echo "[ * ] Flashing to /dev/$$DEV..."; \
	sudo dd if=$(IMAGE) of=/dev/$$DEV bs=4M status=progress; \
	sync
	@echo "[ * ] Done."

clean:
	@echo "[ * ] Cleaning..."
	@rm -rf $(BUILD)

.PHONY: all run image flash clean efi-dir

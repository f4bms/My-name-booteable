ARCH      = x86_64
BOOT_EFI  = BOOTX64.EFI
GAME_EFI  = GAME.EFI
IMAGE     = bootloader.img
IMG_SIZE_MB = 64

CC = clang
LD = lld-link

CFLAGS = \
	-target x86_64-unknown-windows \
	-ffreestanding \
	-fshort-wchar \
	-mno-red-zone \
	-fno-stack-protector \
	-fno-stack-check \
	-Wall -Wextra

LDFLAGS = \
	-subsystem:efi_application \
	-nodefaultlib

all: $(BOOT_EFI) $(GAME_EFI)

$(BOOT_EFI): bootloader.c
	$(CC) $(CFLAGS) -c bootloader.c -o bootloader.o
	$(LD) $(LDFLAGS) -entry:efi_main bootloader.o -out:$(BOOT_EFI)

$(GAME_EFI): game.c
	$(CC) $(CFLAGS) -c game.c -o game.o
	$(LD) $(LDFLAGS) -entry:efi_main game.o -out:$(GAME_EFI)

efi-dir: all
	mkdir -p EFI/BOOT
	cp $(BOOT_EFI) EFI/BOOT/BOOTX64.EFI
	cp $(GAME_EFI) EFI/BOOT/GAME.EFI

run: efi-dir
	qemu-system-x86_64 \
		-bios /usr/share/ovmf/x64/OVMF.4m.fd \
		-drive format=raw,file=fat:rw:. \
		-net none \
		-nographic

image: efi-dir
	dd if=/dev/zero of=$(IMAGE) bs=1M count=$(IMG_SIZE_MB)
	parted -s $(IMAGE) mklabel gpt
	parted -s $(IMAGE) mkpart ESP fat32 1MiB 100%
	parted -s $(IMAGE) set 1 esp on
	@LOOP=$$(sudo losetup -f --show -P $(IMAGE)); \
	sudo mkfs.fat -F32 $${LOOP}p1; \
	sudo mkdir -p /mnt/efi_tmp; \
	sudo mount $${LOOP}p1 /mnt/efi_tmp; \
	sudo mkdir -p /mnt/efi_tmp/EFI/BOOT; \
	sudo cp $(BOOT_EFI) /mnt/efi_tmp/EFI/BOOT/BOOTX64.EFI; \
	sudo cp $(GAME_EFI) /mnt/efi_tmp/EFI/BOOT/GAME.EFI; \
	sudo umount /mnt/efi_tmp; \
	sudo losetup -d $$LOOP

clean:
	rm -f *.o *.EFI $(IMAGE)
	rm -rf EFI

.PHONY: all run image clean efi-dir
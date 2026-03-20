# My-name-booteable

Implementación de un bootloader en dos modos: **Legacy BIOS** (ensamblador x86) y **UEFI** (C con clang), ambos capaces de cargar y ejecutar el juego *My Name*.

---

## El juego

El nombre de los integrantes aparece en una posición aleatoria de la pantalla. Se puede rotar con las flechas, reiniciar y salir.

| Tecla        | Acción                        |
|--------------|-------------------------------|
| ↑            | Horizontal normal             |
| ↓            | Horizontal invertido          |
| →            | Vertical hacia abajo          |
| ←            | Vertical hacia arriba         |
| `R`          | Reiniciar en posición random  |
| `ESC`        | Salir                         |
| `ENTER`      | Confirmar inicio              |

---

## Estructura del proyecto
```
├── myName_Legacy/       # Bootloader BIOS en ensamblador
│   ├── src/
│   │   ├── boot.asm     # Bootloader (sector 0)
│   │   └── myname.asm   # Juego (sector 1+)
│   └── Makefile
│
└── myName_UEFI/         # Bootloader UEFI en C
    ├── src/
    │   ├── bootloader.c # Carga y lanza GAME.EFI
    │   ├── game.c       # El juego
    │   └── efi_headers/ # Headers UEFI (github.com/yoppeh/efi)
    └── Makefile
```

---

## Legacy BIOS

### Dependencias
```bash
# Arch Linux
sudo pacman -S nasm qemu-system-x86 make

# Ubuntu/Debian
sudo apt install nasm qemu-system-x86 make
````

### Uso
```bash
cd myName_Legacy

make          # Ensambla y genera build/boot.img
make run      # Lanza en QEMU (modo legacy)
make clean    # Elimina build/
```

El bootloader se escribe en el sector 0 del disco y el juego a partir del sector 1. QEMU arranca sin firmware UEFI, usando SeaBIOS en modo legacy automáticamente.

---

## UEFI

### Dependencias
```bash
# Arch Linux
sudo pacman -S clang lld qemu-system-x86 ovmf make

# Ubuntu/Debian
sudo apt install clang lld qemu-system-x86 ovmf make
```

### Uso
```bash
cd myName_UEFI

make          # Compila y genera build/bootloader.img
make run      # Lanza en QEMU con OVMF
make flash    # Escribe la imagen en un USB
make clean    # Elimina build/
```

La imagen generada contiene una partición GPT con formato FAT32. El firmware UEFI carga `BOOTX64.EFI`, que a su vez lee `GAME.EFI` desde la misma partición y lo ejecuta.
```
EFI/
└── BOOT/
    ├── BOOTX64.EFI   ← bootloader
    └── GAME.EFI      ← juego
```

### Flash a USB

En `make flash` lista los discos disponibles y pide confirmación antes de escribir. También se puede hacer manualmente:
```bash
sudo dd if=build/bootloader.img of=/dev/sdX bs=4M status=progress # UEFI
```

> **Advertencia:** reemplaza `/dev/sdX` con tu USB. Apunta siempre al disco completo, no a una partición (`sdb` y no `sdb1`). Verifica dos veces con `lsblk` antes de ejecutar.
#define EFI_PLATFORM EFI_ARCH_X64

#include <efi.h>
#include <efi-bs.h>
#include <efi-rs.h>
#include <efi-st.h>
#include <efi-time.h>
#include <protocol/efi-stip.h>
#include <protocol/efi-stop.h>

#define EFI_PLATFORM EFI_ARCH_X64

#ifndef NULL
#define NULL ((VOID*)0)
#endif

#define KEY_UP    0x01
#define KEY_DOWN  0x02
#define KEY_LEFT  0x04
#define KEY_RIGHT 0x03
#define KEY_ESC   0x17

#define ROT_NORMAL 0
#define ROT_FLIP   1
#define ROT_LEFT   2
#define ROT_RIGHT  3

#define MIN_ROW 5
#define MIN_COL 5
#define MAX_ROW 24

static const CHAR16 str_name[] = L"FabiJere";

static UINTN str16len(const CHAR16 *s) {
    UINTN n = 0;
    while (s[n]) n++;
    return n;
}

static void print_char_at(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                           UINTN col, UINTN row, CHAR16 ch)
{
    CHAR16 buf[2] = { ch, 0 };
    ConOut->SetCursorPosition(ConOut, col, row);
    ConOut->OutputString(ConOut, buf);
}

static void print_str_at(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                          UINTN col, UINTN row, const CHAR16 *s)
{
    ConOut->SetCursorPosition(ConOut, col, row);
    ConOut->OutputString(ConOut, (CHAR16*)s);
}

static EFI_INPUT_KEY wait_key(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn,
                               EFI_BOOT_SERVICES *BS)
{
    UINTN Index;
    EFI_INPUT_KEY key;
    BS->WaitForEvent(1, &ConIn->WaitForKey, &Index);
    ConIn->ReadKeyStroke(ConIn, &key);
    return key;
}
// Función para generar una posición aleatoria para el nombre
static void randomize(EFI_RUNTIME_SERVICES *RS,
                      UINT8 *out_row, UINT8 *out_col)
{
    EFI_TIME t;
    RS->GetTime(&t, NULL);
    UINT32 seed = (UINT32)t.Second * 1000 + (t.Nanosecond / 1000000);
    *out_row = (UINT8)(seed % 10) + MIN_ROW;
    *out_col = (UINT8)((seed / 10) % 10) + MIN_COL;
}

static void draw_game(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                      UINT8 rotation, UINT8 name_row, UINT8 name_col)
{
    ConOut->ClearScreen(ConOut);
    print_str_at(ConOut, 0, 0, L"Flechas: rotar | R: reiniciar | ESC: salir");

    UINTN len = str16len(str_name);

    switch (rotation) {
    case ROT_NORMAL:
        print_str_at(ConOut, name_col, name_row, str_name);
        break;
    case ROT_FLIP:
        for (UINTN i = 0; i < len; i++)
            print_char_at(ConOut, name_col + i, name_row, str_name[len - 1 - i]);
        break;
    case ROT_RIGHT:
        for (UINTN i = 0; i < len; i++) {
            UINTN row = name_row + i;
            if (row >= MAX_ROW) break;
            print_char_at(ConOut, name_col, row, str_name[i]);
        }
        break;
    case ROT_LEFT:
        for (UINTN i = 0; i < len; i++) {
            UINTN row = name_row + i;
            if (row >= MAX_ROW) break;
            print_char_at(ConOut, name_col, row, str_name[len - 1 - i]);
        }
        break;
    }
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    (void)ImageHandle;

    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = ST->ConOut;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn  = ST->ConIn;
    EFI_BOOT_SERVICES               *BS     = ST->BootServices;
    EFI_RUNTIME_SERVICES            *RS     = ST->RuntimeServices;

    UINT8 name_row, name_col;
    UINT8 rotation = ROT_NORMAL;

    randomize(RS, &name_row, &name_col); // Generar posición inicial aleatoria para el nombre

    ConOut->ClearScreen(ConOut);
    print_str_at(ConOut, 0, 0, L"Presiona ENTER para comenzar...");
    ConIn->Reset(ConIn, 0);

    while (1) {
        EFI_INPUT_KEY k = wait_key(ConIn, BS);
        if (k.UnicodeChar == L'\r') break;
    }

    draw_game(ConOut, rotation, name_row, name_col);

    while (1) {
        EFI_INPUT_KEY k = wait_key(ConIn, BS);

        if (k.ScanCode == KEY_ESC) break;

        if (k.UnicodeChar == L'r' || k.UnicodeChar == L'R') {
            rotation = ROT_NORMAL;
            randomize(RS, &name_row, &name_col);
            draw_game(ConOut, rotation, name_row, name_col);
            continue;
        }

        if (k.UnicodeChar == 0) {
            switch (k.ScanCode) {
            case KEY_UP:    rotation = ROT_NORMAL; break;
            case KEY_DOWN:  rotation = ROT_FLIP;   break;
            case KEY_LEFT:  rotation = ROT_LEFT;   break;
            case KEY_RIGHT: rotation = ROT_RIGHT;  break;
            default: continue;
            }
            draw_game(ConOut, rotation, name_row, name_col);
        }
    }

    ConOut->ClearScreen(ConOut);
    return EFI_SUCCESS;
}
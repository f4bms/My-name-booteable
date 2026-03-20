typedef unsigned short     CHAR16;
typedef unsigned long long UINT64;
typedef unsigned int       UINT32;
typedef unsigned short     UINT16;
typedef unsigned char      UINT8;
typedef UINT64             UINTN;
typedef void               VOID;
typedef UINTN              EFI_STATUS;
typedef void*              EFI_HANDLE;
typedef void*              EFI_EVENT;

#define EFI_SUCCESS 0

// Scancodes
#define KEY_UP    0x01
#define KEY_DOWN  0x02
#define KEY_LEFT  0x04
#define KEY_RIGHT 0x06
#define KEY_ESC   0x17

#define ROT_NORMAL 0
#define ROT_FLIP   1
#define ROT_LEFT   2
#define ROT_RIGHT  3

#define MIN_ROW 5
#define MIN_COL 5
#define MAX_ROW 24
#define MAX_COL 75

typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_STATUS (*Reset)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, UINT8);
    EFI_STATUS (*ReadKeyStroke)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, EFI_INPUT_KEY*);
    EFI_EVENT  WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (*Reset)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINT8);
    EFI_STATUS (*OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, CHAR16*);
    void*      TestString;
    void*      QueryMode;
    void*      SetMode;
    void*      SetAttribute;
    EFI_STATUS (*ClearScreen)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*);
    EFI_STATUS (*SetCursorPosition)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINTN Col, UINTN Row);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    UINT16 Year;
    UINT8  Month;
    UINT8  Day;
    UINT8  Hour;
    UINT8  Minute;
    UINT8  Second;
    UINT8  Pad1;
    UINT32 Nanosecond;
    UINT16 TimeZone;
    UINT8  Daylight;
    UINT8  Pad2;
} EFI_TIME;

typedef struct {
    void*      GetTime_fn;
    // lo definimos abajo como puntero real
} _pad_rt;

typedef struct {
    UINT64    Signature;
    UINT32    Revision;
    UINT32    HeaderSize;
    UINT32    CRC32;
    UINT32    Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    EFI_STATUS (*GetTime)(EFI_TIME*, void*);
    // resto no nos importa
} EFI_RUNTIME_SERVICES;

typedef struct {
    EFI_TABLE_HEADER                  Hdr;
    void*                             RaiseTPL;
    void*                             RestoreTPL;
    void*                             AllocatePages;
    void*                             FreePages;
    void*                             GetMemoryMap;
    void*                             AllocatePool;
    void*                             FreePool;
    void*                             CreateEvent;
    void*                             SetTimer;
    EFI_STATUS (*WaitForEvent)(UINTN, EFI_EVENT*, UINTN*);
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER                  Hdr;
    CHAR16*                           FirmwareVendor;
    UINT32                            FirmwareRevision;
    UINT32                            _pad;
    EFI_HANDLE                        ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL*   ConIn;
    EFI_HANDLE                        ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*  ConOut;
    EFI_HANDLE                        StandardErrorHandle;
    void*                             StdErr;
    EFI_RUNTIME_SERVICES*             RuntimeServices;
    EFI_BOOT_SERVICES*                BootServices;
} EFI_SYSTEM_TABLE;

// ---------- helpers ----------

static UINTN str16len(const CHAR16 *s) {
    UINTN n = 0;
    while (s[n]) n++;
    return n;
}

// Imprime un solo carácter CHAR16 en la posición (col, row)
static void print_char_at(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                           UINTN col, UINTN row, CHAR16 ch)
{
    CHAR16 buf[2] = { ch, 0 };
    ConOut->SetCursorPosition(ConOut, col, row);
    ConOut->OutputString(ConOut, buf);
}

// Imprime un string CHAR16 en (col, row)
static void print_str_at(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                          UINTN col, UINTN row, const CHAR16 *s)
{
    ConOut->SetCursorPosition(ConOut, col, row);
    ConOut->OutputString(ConOut, (CHAR16*)s);
}

// Espera una tecla y la devuelve
static EFI_INPUT_KEY wait_key(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn,
                               EFI_BOOT_SERVICES *BS)
{
    UINTN Index;
    EFI_INPUT_KEY key;
    BS->WaitForEvent(1, &ConIn->WaitForKey, &Index);
    ConIn->ReadKeyStroke(ConIn, &key);
    return key;
}

// ---------- lógica del juego ----------

static const CHAR16 str_name[] = L"FabiJere";

static void randomize(EFI_RUNTIME_SERVICES *RS,
                      UINT8 *out_row, UINT8 *out_col)
{
    EFI_TIME t;
    RS->GetTime(&t, (void*)0);

    // Usamos segundos y nanosegundos para más entropía
    UINT32 seed = (UINT32)t.Second * 1000 + (t.Nanosecond / 1000000);

    *out_row = (UINT8)(seed % 10) + MIN_ROW;
    *out_col = (UINT8)((seed / 10) % 10) + MIN_COL;
}

static void draw_game(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
                      UINT8 rotation, UINT8 name_row, UINT8 name_col)
{
    ConOut->ClearScreen(ConOut);

    // Instrucciones
    print_str_at(ConOut, 0, 0, L"Flechas: rotar | R: reiniciar | ESC: salir");

    UINTN len = str16len(str_name);

    switch (rotation) {

    case ROT_NORMAL:
        print_str_at(ConOut, name_col, name_row, str_name);
        break;

    // De derecha a izquierda (flip horizontal)
    case ROT_FLIP:
        for (UINTN i = 0; i < len; i++) {
            print_char_at(ConOut,
                          name_col + i,
                          name_row,
                          str_name[len - 1 - i]);
        }
        break;

    // Vertical hacia abajo (→ en el original)
    case ROT_RIGHT:
        for (UINTN i = 0; i < len; i++) {
            UINTN row = name_row + i;
            if (row >= MAX_ROW) break;
            print_char_at(ConOut, name_col, row, str_name[i]);
        }
        break;

    // Vertical hacia arriba (← en el original, letras al revés)
    case ROT_LEFT:
        for (UINTN i = 0; i < len; i++) {
            UINTN row = name_row + i;
            if (row >= MAX_ROW) break;
            print_char_at(ConOut, name_col, row, str_name[len - 1 - i]);
        }
        break;
    }
}

// ---------- entry point ----------

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST)
{
    (void)ImageHandle;

    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = ST->ConOut;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn  = ST->ConIn;
    EFI_BOOT_SERVICES               *BS     = ST->BootServices;
    EFI_RUNTIME_SERVICES            *RS     = ST->RuntimeServices;

    UINT8 name_row, name_col;
    UINT8 rotation = ROT_NORMAL;

    randomize(RS, &name_row, &name_col);

    // Esperar ENTER para comenzar (igual que el original)
    ConOut->ClearScreen(ConOut);
    print_str_at(ConOut, 0, 0, L"Presiona ENTER para comenzar...");
    ConIn->Reset(ConIn, 0);

    while (1) {
        EFI_INPUT_KEY k = wait_key(ConIn, BS);
        if (k.UnicodeChar == L'\r') break;
    }

    draw_game(ConOut, rotation, name_row, name_col);

    // Loop principal
    while (1) {
        EFI_INPUT_KEY k = wait_key(ConIn, BS);

        // ESC
        if (k.ScanCode == KEY_ESC) break;

        // R = reiniciar
        if (k.UnicodeChar == L'r' || k.UnicodeChar == L'R') {
            rotation = ROT_NORMAL;
            randomize(RS, &name_row, &name_col);
            draw_game(ConOut, rotation, name_row, name_col);
            continue;
        }

        // Flechas
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
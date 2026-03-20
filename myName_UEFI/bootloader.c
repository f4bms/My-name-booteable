typedef unsigned short     CHAR16;
typedef unsigned long long UINT64;
typedef unsigned int       UINT32;
typedef unsigned short     UINT16;
typedef unsigned char      UINT8;
typedef UINT64             UINTN;
typedef UINT64             EFI_STATUS;
typedef void*              EFI_HANDLE;
typedef void*              EFI_EVENT;
typedef UINT64             EFI_LBA;
typedef UINT64             EFI_PHYSICAL_ADDRESS;

#define EFI_SUCCESS              0ULL
#define EFI_LOAD_ERROR           (0x8000000000000001ULL)
#define EFI_INVALID_PARAMETER    (0x8000000000000002ULL)
#define EFI_NOT_FOUND            (0x800000000000000EULL)

// ---------- GUIDs ----------

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

// EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID
#define SIMPLE_FS_GUID \
    { 0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} }

// EFI_LOADED_IMAGE_PROTOCOL_GUID
#define LOADED_IMAGE_GUID \
    { 0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B} }

// ---------- Device Path ----------

typedef struct {
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

// ---------- File Protocol ----------

typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;

struct _EFI_FILE_PROTOCOL {
    UINT64     Revision;
    EFI_STATUS (*Open)(EFI_FILE_PROTOCOL*, EFI_FILE_PROTOCOL**,
                       CHAR16*, UINT64, UINT64);
    EFI_STATUS (*Close)(EFI_FILE_PROTOCOL*);
    void*      Delete;
    EFI_STATUS (*Read)(EFI_FILE_PROTOCOL*, UINTN*, void*);
    EFI_STATUS (*Write)(EFI_FILE_PROTOCOL*, UINTN*, void*);
    EFI_STATUS (*GetPosition)(EFI_FILE_PROTOCOL*, UINT64*);
    EFI_STATUS (*SetPosition)(EFI_FILE_PROTOCOL*, UINT64);
    EFI_STATUS (*GetInfo)(EFI_FILE_PROTOCOL*, EFI_GUID*, UINTN*, void*);
};

// EFI_FILE_INFO_ID
#define FILE_INFO_GUID \
    { 0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} }

typedef struct {
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    UINT8  _times[48]; // CreateTime, LastAccessTime, ModificationTime
    UINT64 Attribute;
    CHAR16 FileName[256];
} EFI_FILE_INFO;

// ---------- Simple File System ----------

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64     Revision;
    EFI_STATUS (*OpenVolume)(struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*,
                             EFI_FILE_PROTOCOL**);
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// ---------- Loaded Image ----------

typedef struct {
    UINT32                    Revision;
    EFI_HANDLE                ParentHandle;
    EFI_HANDLE                SystemTable;
    EFI_HANDLE                DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL* FilePath;
    void*                     Reserved;
    UINT32                    LoadOptionsSize;
    void*                     LoadOptions;
    void*                     ImageBase;
    UINT64                    ImageSize;
    UINT32                    ImageCodeType;
    UINT32                    ImageDataType;
    EFI_STATUS (*Unload)(EFI_HANDLE);
} EFI_LOADED_IMAGE_PROTOCOL;

// ---------- Protocolos de ConOut/ConIn ----------

typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_STATUS (*Reset)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, UINT8);
    EFI_STATUS (*ReadKeyStroke)(struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL*,
                                EFI_INPUT_KEY*);
    EFI_EVENT  WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (*Reset)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINT8);
    EFI_STATUS (*OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*,
                               CHAR16*);
    void*      TestString;
    void*      QueryMode;
    void*      SetMode;
    void*      SetAttribute;
    EFI_STATUS (*ClearScreen)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*);
    EFI_STATUS (*SetCursorPosition)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*,
                                    UINTN, UINTN);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// ---------- Table Header ----------

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

// ---------- Runtime Services ----------

typedef struct {
    EFI_TABLE_HEADER Hdr;
    EFI_STATUS (*GetTime)(void*, void*);
} EFI_RUNTIME_SERVICES;

// ---------- Boot Services ----------

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* RaiseTPL;
    void* RestoreTPL;
    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    EFI_STATUS (*AllocatePool)(UINTN, UINTN, void**);
    EFI_STATUS (*FreePool)(void*);
    void* CreateEvent;
    void* SetTimer;
    EFI_STATUS (*WaitForEvent)(UINTN, EFI_EVENT*, UINTN*);
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;
    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    EFI_STATUS (*HandleProtocol)(EFI_HANDLE, EFI_GUID*, void**);
    void* Reserved;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;
    EFI_STATUS (*LoadImage)(UINT8, EFI_HANDLE, EFI_DEVICE_PATH_PROTOCOL*,
                            void*, UINTN, EFI_HANDLE*);
    EFI_STATUS (*StartImage)(EFI_HANDLE, UINTN*, CHAR16**);
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;
} EFI_BOOT_SERVICES;

// ---------- System Table ----------

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

// ---------- cargar y ejecutar GAME.EFI ----------

static EFI_STATUS launch_game(EFI_HANDLE ImageHandle,
                               EFI_SYSTEM_TABLE *ST)
{
    EFI_BOOT_SERVICES *BS = ST->BootServices;
    EFI_STATUS status;

    // 1. Obtener el dispositivo desde el que arrancamos nosotros
    EFI_GUID li_guid = LOADED_IMAGE_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *li = (void*)0;
    status = BS->HandleProtocol(ImageHandle, &li_guid, (void**)&li);
    if (status != EFI_SUCCESS) return status;

    // 2. Abrir el sistema de archivos de ese dispositivo
    EFI_GUID fs_guid = SIMPLE_FS_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = (void*)0;
    status = BS->HandleProtocol(li->DeviceHandle, &fs_guid, (void**)&fs);
    if (status != EFI_SUCCESS) return status;

    // 3. Abrir el volumen (raíz de la partición FAT32)
    EFI_FILE_PROTOCOL *root = (void*)0;
    status = fs->OpenVolume(fs, &root);
    if (status != EFI_SUCCESS) return status;

    // 4. Abrir GAME.EFI
    EFI_FILE_PROTOCOL *game_file = (void*)0;
    status = root->Open(root, &game_file,
                        L"\\EFI\\BOOT\\GAME.EFI",
                        0x0000000000000001ULL, // EFI_FILE_MODE_READ
                        0);
    if (status != EFI_SUCCESS) return status;

    // 5. Obtener tamaño del archivo via GetInfo
    EFI_GUID fi_guid = FILE_INFO_GUID;
    UINT8 info_buf[sizeof(EFI_FILE_INFO) + 512];
    UINTN info_size = sizeof(info_buf);
    status = game_file->GetInfo(game_file, &fi_guid, &info_size, info_buf);
    if (status != EFI_SUCCESS) return status;

    EFI_FILE_INFO *fi = (EFI_FILE_INFO*)info_buf;
    UINTN file_size = (UINTN)fi->FileSize;

    // 6. Leer el archivo completo en memoria
    void *file_buf = (void*)0;
    status = BS->AllocatePool(2 /*EfiRuntimeServicesData*/, file_size, &file_buf);
    if (status != EFI_SUCCESS) return status;

    status = game_file->Read(game_file, &file_size, file_buf);
    game_file->Close(game_file);
    root->Close(root);
    if (status != EFI_SUCCESS) return status;

    // 7. Cargar la imagen EFI desde el buffer en memoria
    EFI_HANDLE game_handle = (void*)0;
    status = BS->LoadImage(
        0,              // BootPolicy = FALSE
        ImageHandle,    // parent
        (void*)0,       // DevicePath = NULL (cargamos desde buffer)
        file_buf,
        file_size,
        &game_handle
    );
    if (status != EFI_SUCCESS) return status;

    // 8. Ejecutar
    UINTN exit_data_size = 0;
    CHAR16 *exit_data = (void*)0;
    status = BS->StartImage(game_handle, &exit_data_size, &exit_data);

    return status;
}

// ---------- entry point ----------

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST)
{
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = ST->ConOut;

    ConOut->ClearScreen(ConOut);
    ConOut->OutputString(ConOut, L"Cargando juego...\r\n");

    EFI_STATUS status = launch_game(ImageHandle, ST);

    if (status != EFI_SUCCESS) {
        CHAR16 code[21];
        ConOut->ClearScreen(ConOut);
        ConOut->OutputString(ConOut, L"Error al cargar GAME.EFI: 0x");

        // Quedar colgado para que puedas leer el error
        while (1) {}
    }

    return EFI_SUCCESS;
}
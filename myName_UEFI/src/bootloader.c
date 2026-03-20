#define EFI_PLATFORM EFI_ARCH_X64

#include <efi.h>
#include <efi-bs.h>
#include <efi-st.h>
#include <protocol/efi-lip.h>
#include <protocol/efi-sfsp.h>
#include <protocol/efi-fp.h>
#include <protocol/efi-stop.h>

#define EFI_PLATFORM EFI_ARCH_X64

#ifndef NULL
#define NULL ((VOID*)0)
#endif


static EFI_STATUS launch_game(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    EFI_BOOT_SERVICES *BS = ST->BootServices;
    EFI_STATUS status;

    // Obtener dispositivo de arranque
    EFI_GUID li_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *li = NULL;
    status = BS->HandleProtocol(ImageHandle, &li_guid, (VOID**)&li); // Obtener protocolo de imagen cargada
    if (status != EFI_SUCCESS) return status;

    // Abrir sistema de archivos
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    status = BS->HandleProtocol(li->DeviceHandle, &fs_guid, (VOID**)&fs); // Obtener protocolo de sistema de archivos
    if (status != EFI_SUCCESS) return status;

    // Abrir volumen
    EFI_FILE_PROTOCOL *root = NULL;
    status = fs->OpenVolume(fs, &root); // Abrir volumen raíz
    if (status != EFI_SUCCESS) return status;

    // Abrir GAME.EFI
    EFI_FILE_PROTOCOL *game_file = NULL;
    status = root->Open(root, &game_file,
                        L"\\EFI\\BOOT\\GAME.EFI",
                        EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) return status;

    // Obtener tamaño del archivo
    EFI_GUID fi_guid = EFI_FILE_INFO_ID;
    UINT8 info_buf[512];
    UINTN info_size = sizeof(info_buf);
    status = game_file->GetInfo(game_file, &fi_guid, &info_size, info_buf); // Obtener información del archivo
    if (status != EFI_SUCCESS) return status;

    EFI_FILE_INFO *fi = (EFI_FILE_INFO*)info_buf;
    UINTN file_size = (UINTN)fi->FileSize;

    // Leer archivo en memoria
    VOID *file_buf = NULL;
    status = BS->AllocatePool(EfiLoaderData, file_size, &file_buf); // Reservar memoria para el archivo
    if (status != EFI_SUCCESS) return status;

    status = game_file->Read(game_file, &file_size, file_buf); // Leer el archivo en memoria
    game_file->Close(game_file); // Cerrar archivo
    root->Close(root); // Cerrar volumen
    if (status != EFI_SUCCESS) return status;

    // Cargar imagen
    EFI_HANDLE game_handle = NULL;
    status = BS->LoadImage(0, ImageHandle, NULL, file_buf, file_size, &game_handle); // Cargar la imagen en memoria
    if (status != EFI_SUCCESS) return status;

    // Ejecutar
    UINTN exit_data_size = 0;
    CHAR16 *exit_data = NULL;
    status = BS->StartImage(game_handle, &exit_data_size, &exit_data); // Ejecutar la imagen

    BS->FreePool(file_buf); // Liberar memoria del archivo
    return status;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = ST->ConOut;

    ConOut->ClearScreen(ConOut);
    ConOut->OutputString(ConOut, L"Cargando juego...\r\n");

    EFI_STATUS status = launch_game(ImageHandle, ST);

    if (status != EFI_SUCCESS) {
        ConOut->ClearScreen(ConOut);
        ConOut->OutputString(ConOut, L"Error al cargar GAME.EFI\r\n");
        while (1) {}
    }

    return EFI_SUCCESS;
}
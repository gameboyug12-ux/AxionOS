#include <efi.h>
#include <efilib.h>

#define KERNEL_PATH L"\\boot\\vmlinuz"
#define INITRD_PATH L"\\boot\\initrd.img"

void *kernel;
void *initrd;
UINTN kernel_size;
UINTN initrd_size;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    Print(L"MyOS UEFI Bootloader Starting...\n");

    EFI_FILE_IO_INTERFACE *fs;
    EFI_FILE_HANDLE root;

    // get filesystem
    uefi_call_wrapper(
        SystemTable->BootServices->HandleProtocol,
        3,
        ImageHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (void**)&fs
    );

    fs->OpenVolume(fs, &root);

    // LOAD KERNEL
    EFI_FILE_HANDLE kernel_file;
    root->Open(root, &kernel_file, KERNEL_PATH, EFI_FILE_MODE_READ, 0);

    EFI_FILE_INFO *info;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 200;

    uefi_call_wrapper(
        SystemTable->BootServices->AllocatePool,
        3,
        EfiLoaderData,
        info_size,
        (void**)&info
    );

    kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &info_size, info);

    kernel_size = info->FileSize;

    SystemTable->BootServices->AllocatePool(EfiLoaderData, kernel_size, &kernel);

    kernel_file->Read(kernel_file, &kernel_size, kernel);

    kernel_file->Close(kernel_file);

    Print(L"Kernel loaded: %d bytes\n", kernel_size);

    // LOAD INITRD
    EFI_FILE_HANDLE initrd_file;
    root->Open(root, &initrd_file, INITRD_PATH, EFI_FILE_MODE_READ, 0);

    info_size = sizeof(EFI_FILE_INFO) + 200;

    SystemTable->BootServices->AllocatePool(
        EfiLoaderData,
        info_size,
        (void**)&info
    );

    initrd_file->GetInfo(initrd_file, &gEfiFileInfoGuid, &info_size, info);

    initrd_size = info->FileSize;

    SystemTable->BootServices->AllocatePool(EfiLoaderData, initrd_size, &initrd);

    initrd_file->Read(initrd_file, &initrd_size, initrd);

    initrd_file->Close(initrd_file);

    Print(L"Initrd loaded: %d bytes\n", initrd_size);

    Print(L"Jumping to kernel...\n");

    // ⚠️ OVO JE SIMPLIFIKOVANO
    typedef void (*kernel_entry)(void*, void*);
    kernel_entry entry = (kernel_entry)kernel;

    entry(kernel, initrd);

    while (1);

    return EFI_SUCCESS;
}

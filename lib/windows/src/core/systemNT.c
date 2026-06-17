#include <linux/fs.h>
#include <linux/types.h>
typedef int NTSTATUS;
typedef void* HANDLE;

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

NTSTATUS NtCreateFile(const char* path, HANDLE* out)
{
    struct file* f = filp_open(path, O_RDONLY, 0);

    if (IS_ERR(f))
        return STATUS_FAIL;

    *out = (HANDLE)f;
    return STATUS_SUCCESS;
}

NTSTATUS NtClose(HANDLE h)
{
    filp_close((struct file*)h, NULL);
    return STATUS_SUCCESS;
}
typedef struct nt_driver {
    int (*DriverEntry)(void* DriverObject);
    int (*Unload)(void* DriverObject);
} nt_driver_t;
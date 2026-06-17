#ifndef NT_H
#define NT_H

typedef void* HANDLE;
typedef int NTSTATUS;

NTSTATUS NtCreateFile(const char* path);
NTSTATUS NtReadFile(HANDLE h, void* buf, int size);
void NtClose(HANDLE h);

#endif
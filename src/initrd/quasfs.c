#include <initrd/quasfs.h>
#include <heap/heap.h>

quasHead* QuasFS_Head;
char* QuasFS_Addr;

int QuasFS_Init(void* addr) {
    QuasFS_Addr = (char*)addr;
    QuasFS_Head = (quasHead*)QuasFS_Addr;
    if (strcmp(QuasFS_Head->magic, "QUAS")) {
        return 1;
    }
    return 0;
}

char* QuasFS_Read(const char* name) {
    quasHdr* hdr;
    for (u64 i = 0; i < QuasFS_Head->fileCount; i++) {
        hdr = (quasHdr*)(QuasFS_Addr + (sizeof(quasHead) + (sizeof(quasHdr) * i)));
        if (!strcmp(hdr->name, name)) {
            return (char*)(QuasFS_Addr + hdr->address);
        }
    }
    return NULL;
}

size_t QuasFS_FTell(const char* name) {
    quasHdr* hdr;
    for (u64 i = 0; i < QuasFS_Head->fileCount; i++) {
        hdr = (quasHdr*)(QuasFS_Addr + (sizeof(quasHead) + (sizeof(quasHdr) * i)));
        if (!strcmp(hdr->name, name)) {
            return hdr->size;
        }
    }
    return 0;
}

char** QuasFS_Dir() {
    quasHdr* hdr;
    char** res = (char**)Heap_Alloc(sizeof(char) * QuasFS_Head->fileCount);
    for (u64 i = 0; i < QuasFS_Head->fileCount; i++) {
        hdr = (quasHdr*)(QuasFS_Addr + (sizeof(quasHead) + (sizeof(quasHdr) * i)));
        res[i] = (char*)Heap_Alloc(sizeof(char) * 50);
        strcpy(res[i], hdr->name);
    }
    return res;
}

int QuasFS_GetEntryCount() {
    return QuasFS_Head->fileCount;
}
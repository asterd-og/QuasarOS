#include <initrd/quasfs.h>
#include <heap/heap.h>

quasfs_head* quasfs_Head;
char* quasfs_Addr;

int quasfs_init(void* addr) {
    quasfs_Addr = (char*)addr;
    quasfs_Head = (quasfs_head*)quasfs_Addr;
    if (strcmp(quasfs_Head->magic, "QUAS")) {
        return 1;
    }
    return 0;
}

char* quasfs_read(const char* name) {
    quasfs_header* hdr;
    for (u64 i = 0; i < quasfs_Head->fileCount; i++) {
        hdr = (quasfs_header*)(quasfs_Addr + (sizeof(quasfs_head) + (sizeof(quasfs_header) * i)));
        if (!strcmp(hdr->name, name)) {
            return (char*)(quasfs_Addr + hdr->address);
        }
    }
    return NULL;
}

size_t quasfs_ftell(const char* name) {
    quasfs_header* hdr;
    for (u64 i = 0; i < quasfs_Head->fileCount; i++) {
        hdr = (quasfs_header*)(quasfs_Addr + (sizeof(quasfs_head) + (sizeof(quasfs_header) * i)));
        if (!strcmp(hdr->name, name)) {
            return hdr->size;
        }
    }
    return 0;
}

char** quasfs_dir() {
    quasfs_header* hdr;
    char** res = (char**)kmalloc(sizeof(char) * quasfs_Head->fileCount);
    for (u64 i = 0; i < quasfs_Head->fileCount; i++) {
        hdr = (quasfs_header*)(quasfs_Addr + (sizeof(quasfs_head) + (sizeof(quasfs_header) * i)));
        res[i] = (char*)kmalloc(sizeof(char) * 50);
        strcpy(res[i], hdr->name);
    }
    return res;
}

int quasfs_get_entry_count() {
    return quasfs_Head->fileCount;
}
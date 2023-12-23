#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC 0xbadcafee

typedef struct {
    char magic[4];
    size_t fileCount;
} quasHead;

typedef struct {
    uint64_t magic; // should be 0xbadcafee
    char name[50];
    uint64_t address;
    size_t size;
} quasHdr;

int main(int argc, char** argv) {
    FILE* fp = fopen(argv[1], "r");
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    char* buffer = (char*)malloc(size);
    rewind(fp);
    fread(buffer, 1, size, fp);
    quasHead* head = (quasHead*)buffer;
    printf("%s %ld\n", head->magic, head->fileCount);
    quasHdr* header;
    if (head->fileCount > 0) {
        for (uint64_t i = 0; i < head->fileCount; i++) {
            header = (quasHdr*)(buffer + (sizeof(quasHead) + (sizeof(quasHdr) * i)));
            if (header->magic != 0xbadcafee) {
                printf("Invalid file header.\n");
                fclose(fp);
                free(head);
                exit(1);
            }
            printf("%s\n", header->name);
            //printf("- %s\n", buffer + header->address);
        }
    }

    fclose(fp);
    free(head);

    return 0;
}
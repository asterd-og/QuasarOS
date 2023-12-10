#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
    size_t headersCount = argc - 1;
    quasHdr* headers = (quasHdr*)malloc(sizeof(quasHdr) * headersCount);
    uint64_t address = (sizeof(quasHdr) * headersCount) + sizeof(quasHead);

    FILE* fp;

    for (int i = 0; i < headersCount; i++) {
        headers[i].magic = 0xbadcafee;
        strcpy(headers[i].name, argv[i + 1]);
        fp = fopen(argv[i + 1], "r");
        fseek(fp, 0L, SEEK_END);
        headers[i].size = ftell(fp);
        rewind(fp);
        fclose(fp);
        headers[i].address = address;
        address += headers[i].size;
    }

    fp = fopen("./quasfs.fs", "w");
    char* data = (char*)malloc(address);
    quasHead head;
    strcpy(head.magic, "QUAS");
    head.fileCount = headersCount;
    fwrite(&head, sizeof(quasHead), 1, fp);
    fwrite(headers, sizeof(quasHdr), headersCount, fp);
    FILE* stream;
    char* tmpBuf;
    for (int i = 0; i < headersCount; i++) {
        printf("Writing file %s\n", headers[i].name);
        stream = fopen(argv[i + 1], "r");
        tmpBuf = (char*)malloc(headers[i].size);
        fread(tmpBuf, 1, headers[i].size, stream);
        fwrite(tmpBuf, 1, headers[i].size, fp);
        fclose(stream);
        free(tmpBuf);
    }

    fclose(fp);
    free(data);

    return 0;
}
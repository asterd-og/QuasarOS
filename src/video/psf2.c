#include <video/psf2.h>
#include <heap/heap.h>

psf2_font* psf2_load(char* font_addr) {
    psf2_header* hdr = (psf2_header*)font_addr;
    if (hdr->magic[0] != 0x72 || hdr->magic[1] != 0xb5 || hdr->magic[2] != 0x4a || hdr->magic[3] != 0x86) {
        serial_printf("Invalid PSF2 Header!");
        return NULL;
    }
    psf2_font* font = (psf2_font*)kmalloc(sizeof(psf2_font));
    font->header = hdr;
    font->pitch = hdr->char_size / hdr->height;
    font->addr_start = (u8*)(font_addr + hdr->header_size);
    return font;
}
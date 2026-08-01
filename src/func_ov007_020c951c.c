#include "types.h"
extern void *data_ov007_02104c20;
extern void *data_ov007_02104c24;
extern void *data_ov007_02104c1c;
extern void *data_ov007_02104c18;
struct Header { u32 pad[3]; u32 count; };
struct Chunk { u32 magic; u32 size; };
void func_ov007_020c951c(int unused, struct Header *h)
{
    u32 i;
    char *p = (char *)h;
    data_ov007_02104c20 = h;
    p += 0x20;
    for (i = 0; i < h->count; i++) {
        switch (((struct Chunk *)p)->magic) {
        case 0x494e4631: data_ov007_02104c1c = p; data_ov007_02104c18 = p + 0x10; break;
        case 0x464c5731: break;
        case 0x44415431: data_ov007_02104c24 = p + 8; break;
        case 0x53545231: break;
        case 0x464c4931: break;
        }
        p += ((struct Chunk *)p)->size;
    }
}

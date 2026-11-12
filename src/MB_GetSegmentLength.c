#include "nitro/fs/file.h"

struct G {
    char pad[0x2c];
    int f2c; /* 0x2c */
    char pad30[0x3c - 0x30];
    int f3c; /* 0x3c */
};

int MB_GetSegmentLength(FSFile *obj) {
    char buf[0x60];
    struct G *g = 0;
    int len;
    if (obj != 0) {
        len = obj->prop.file.pos - obj->prop.file.top; /* FS_GetPosition */
        if ((unsigned int)FS_ReadFile(obj, buf, 0x60) >= 0x60)
            g = (struct G*)buf;
        FS_SeekFile(obj, len, FS_SEEK_SET);
    } else {
        g = (struct G*)0x27ffe00;
    }
    if (g != 0)
        return g->f2c + 0x1e8 + g->f3c;
    return 0;
}

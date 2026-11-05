#include "private/fs_views.h"

extern int FS_ReadFile(void *obj, void *buf, int size);
extern int func_0205d368(void *obj, int len, int sel);

struct G {
    char pad[0x2c];
    int f2c;    /* 0x2c */
    char pad30[0x3c - 0x30];
    int f3c;    /* 0x3c */
};

int func_02067e70(struct FSFile *obj) {
    char buf[0x60];
    struct G *g = 0;
    int len;
    if (obj != 0) {
        len = obj->prop.file.pos - obj->prop.file.top; /* FS_GetPosition */
        if ((unsigned int)FS_ReadFile(obj, buf, 0x60) >= 0x60)
            g = (struct G*)buf;
        func_0205d368(obj, len, 0);
    } else {
        g = (struct G*)0x27ffe00;
    }
    if (g != 0)
        return g->f2c + 0x1e8 + g->f3c;
    return 0;
}

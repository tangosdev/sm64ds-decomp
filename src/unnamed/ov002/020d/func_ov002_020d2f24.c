#include "types.h"
extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, u32 a, int b);

void func_ov002_020d2f24(char* thiz)
{
    int v;
    int r4;
    u32 id;

    id = *(u32*)(thiz + 8);
    if (id != 3) return;

    r4 = *(int*)(thiz + 0x56c);
    id = _ZNK6Player14GetBodyModelIDEjb(thiz, id & 0xff, 0);
    {
        int* q = (int*)(*(int*)(thiz + id * 4 + 0xdc) + 0x50);
        v = (u32)(q[2] << 4) >> 0x10;
    }

    if (v >= 6) {
        if (v <= 0x1c) {
            r4 -= 0x180;
            goto store;
        }
    }
    if (v >= 0x24 && v <= 0x3a) {
        if (r4 < 0x2800) r4 += 0x180;
        else r4 += 0x300;
        goto store;
    }
    if (v == 0x3b) {
        if (*(u8*)(thiz + 0x70c) != 0) r4 += 0x10;
    }

store:
    if (r4 < 0x400) r4 = 0x400;
    if (r4 > 0x4000) r4 = 0x4000;
    *(int*)(thiz + 0x56c) = r4;
    *(int*)(thiz + 0x570) = r4;
    *(int*)(thiz + 0x574) = r4;
}

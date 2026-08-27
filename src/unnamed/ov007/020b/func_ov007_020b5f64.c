#include "types.h"
extern u8 data_ov007_0210342c[];

void func_ov007_020b5f64(char *self)
{
    char *p;
    char *g;
    int state;
    int x;
    int sel;
    int y;
    char *q;
    int w;

    p = *(char**)self;
    state = *(u16*)p - 0xe;
    g = *(char**)data_ov007_0210342c;
    sel = *(short*)(*(char**)(g + 8));
    x = *(short*)(self + 0x18);
    y = *(short*)(self + 0x1a);

    switch (sel) {
    case 0:
        x = *(short*)(*(char**)(p + 0x24) + 2);
        y = *(short*)(*(char**)(p + 0x24) + 4);
        break;
    case 1:
        y = 0x24;
        break;
    case 3:
    case 6:
        y = 0x58;
        break;
    case 7:
        if (state == *(int*)(g + 0x58) || state == *(int*)(g + 0x5c))
            y = 0x24;
        else
            y = 0x58;
        break;
    case 9:
        if (state == *(int*)(g + 0x58))
            y = 0x24;
        else
            y = 0x58;
        break;
    }

    if (*(int*)(*(char**)(p + 4) + 4) == 2)
        *(int*)(*(char**)(p + 4) + 4) = 0;

    if (x != *(short*)(self + 0x18) || y != *(short*)(self + 0x1a)) {
        *(short*)(self + 0x14) = (short)(*(int*)(p + 8) >> 12);
        *(short*)(self + 0x16) = (short)(*(int*)(p + 0xc) >> 12);
        *(short*)(self + 0x18) = x;
        *(short*)(self + 0x1a) = y;
        q = *(char**)(self + 0x10);
        *(int*)q = 1;
        *(int*)(q + 4) = 1;
        *(short*)(q + 0xc) = 0;
        *(int*)(q + 8) = 0xa0;
        *(int*)(q + 0x14) = 0;
        *(int*)(*(char**)(q + 0x10)) = *(int*)(q + 0x14);
        *(int*)(q + 0x18) = 0x1000;
    } else if (*(int*)(*(char**)(self + 0x10)) == 3) {
        if (*(int*)(*(char**)(p + 4) + 4) == 1)
            *(int*)(*(char**)(p + 4) + 4) = 2;
    }

    if (*(int*)(*(char**)(self + 0x10)) == 3) return;

    w = *(int*)(self + 0xc);
    *(int*)(p + 8) = (int)(((s64)(0x1000 - w) * ((int)*(short*)(self + 0x14) << 12)
                          + (s64)w * ((int)*(short*)(self + 0x18) << 12)) >> 12);
    *(int*)(p + 0xc) = (int)(((s64)(0x1000 - w) * ((int)*(short*)(self + 0x16) << 12)
                            + (s64)w * ((int)*(short*)(self + 0x1a) << 12)) >> 12);
}

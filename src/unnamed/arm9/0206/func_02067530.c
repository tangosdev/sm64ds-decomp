#include "types.h"
extern char* data_020a9db8;

struct Out { int f0; u32 f4; int f8; u8 fc; };

int func_02067530(struct Out* o, char* in, u32 val, char* tbl)
{
    int i;
    if (val >= *(u16*)(in + 0x12)) return 0;
    i = 2;
    for (;;) {
        if (val >= *(u16*)(in + 0xc + i*2)) break;
        i = (signed char)(i - 1);
        if (i < 0) break;
    }
    if (i < 0) return 0;
    {
        int span = *(int*)(data_020a9db8 + 0x1000 + 0x318);
        int d = val - *(u16*)(in + 0xc + i*2);
        int rel = d * span;
        int* row = (int*)(tbl + 0xc + i*0x10);
        o->f4 = row[2] - rel;
        {
            u32 lim = *(u32*)(data_020a9db8 + 0x1000 + 0x318);
            if (o->f4 > lim) o->f4 = lim;
        }
        o->f8 = rel + *(int*)(in + i*4);
        o->f0 = rel + row[0];
        o->fc = (u8)i;
    }
    return 1;
}

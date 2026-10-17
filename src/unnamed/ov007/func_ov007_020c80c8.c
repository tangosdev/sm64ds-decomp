#include "nitro/fx/fx.h"
void func_ov007_020c80c8(char *o, int a, int s)
{
    if (!(*(int *)(o + 0x20) & 1)) {
        if (*(short *)(o + 0x1c) == 0x1000) {
            int m = FX_MUL(a, *(int *)(o + 0xc));
            int *p = (int *)(int)(o + 0xc);
            *p += (*(int *)(o + 0x18) - m) >> s;
        } else {
            int m = FX_MUL(a, *(int *)(o + 0xc));
            int *p18 = (int *)(int)(o + 0x18);
            int m2;
            int *p0c;
            *p18 -= m;
            m2 = FX_MUL(*(int *)(o + 0x18), (*(short *)(o + 0x1c) >> s));
            p0c = (int *)(int)((char *)o + 0xc);
            *p0c += m2;
        }
        {
            int *base = *(int **)o;
            int *q = (int *)(int)((char *)base + 8);
            *q += *(int *)(o + 0xc) >> s;
        }
    }
    *(int *)(o + 0x18) = 0;
}

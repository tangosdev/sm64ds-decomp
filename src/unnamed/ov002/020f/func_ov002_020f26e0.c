#include "types.h"
struct OamAttr;

struct OamEnt {
    s16 x;        /* 0 */
    s16 y;        /* 2 */
    s16 ang;      /* 4 */
    s16 pad;      /* 6 */
    int enabled;  /* 8 */
};

extern struct OamAttr *data_ov002_0210b6a8[];

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int a, struct OamAttr *b, int c, int d, int e, int f, int g, int h);

void func_ov002_020f26e0(void *thiz)
{
    struct OamEnt *e = (struct OamEnt *)((char *)thiz + 0x208);
    int i;
    for (i = 0; i < 0x40; i++, e++) {
        int idx;
        int s;
        if (e->enabled == 0)
            continue;
        idx = 0;
        if (e->ang & 0x80)
            idx = 1;
        s = e->ang / 512;
        if (s >= 7)
            s = 7;
        if (s == 0)
            continue;
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            1, data_ov002_0210b6a8[idx], e->x, e->y,
            -1, 1, ((7 - s) << 10) + 0x1000, 0);
    }
}

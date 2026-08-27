#include "types.h"
struct Inner {
    s32 x;      /* 0x00 */
    s32 y;      /* 0x04 */
    s32 flag;   /* 0x08 */
};

struct Outer {
    struct Inner *inner; /* 0x00 */
    u8 unk04[6];         /* 0x04 */
    u8 margin;           /* 0x0a */
};
void func_ov007_020c3fe4(struct Outer *self)
{
    s32 y;
    s32 m;
    s32 x;
    s32 vis;
    s32 onX;
    s32 negM;
    struct Inner *p;
    s32 f;

    p = self->inner;
    m = self->margin;
    x = p->x >> 12;
    vis = 0;
    onX = 0;
    negM = -m;
    y = p->y >> 12;

    if (negM <= x && x <= m + 255) onX = 1;
    if (onX && negM <= y && y <= m + 191) vis = 1;

    f = p->flag;
    if ((f == 0 && vis) || (f != 0 && !vis)) p->flag = (f == 0);
}

#include "types.h"
struct Anim {
    char pad[0x20];
    int mode;     /* 0x20 */
    int rev;      /* 0x24 */
    int f28;      /* 0x28 */
    short frame;  /* 0x2c */
    short tick;   /* 0x2e */
    int changed;  /* 0x30 */
};

struct AnimData {
    int w0;
    u16 *durations; /* 0x4 */
    int w8;
    u32 count;      /* 0xc */
};

#define AT16(p, off) (*(short *)(int)((char *)(p) + (off)))

void func_ov007_020c0d70(struct Anim *a, struct AnimData *d)
{
    int loop;
    int step;
    short m;
    int dur;

    if (a->mode == 0)
        return;
    if (a->f28 != 0)
        return;

    step = 1;
    loop = (a->mode == 1);
    a->f28 = 0;
    a->changed = 0;
    if (a->rev == 1)
    {
        m = -1;
        step = step * m;
    }

    AT16(a, 0x2e) += step;

    dur = d->durations[a->frame];
    if (a->tick >= dur)
    {
        a->tick = 0;
        AT16(a, 0x2c) += 1;
        if (a->frame < d->count)
            return;
        a->frame = loop ? 0 : d->count - 1;
        a->changed = 1;
    }
    else if (a->tick < 0)
    {
        a->tick = dur - 1;
        AT16(a, 0x2c) -= 1;
        if (a->frame >= 0)
            return;
        a->frame = loop ? d->count - 1 : 0;
        a->changed = 1;
    }
}

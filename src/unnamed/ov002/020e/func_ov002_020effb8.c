#include "types.h"
// @symbol func_ov002_020effb8
/* recovered: shared common types */
#include "common.h"
#define FXMUL(a, b) ((s32)(u32)(((((long long)(a)) * (b)) + 0x800) >> 12))
#define TERM(v, w) ((FXMUL(v, w) + 8) >> 4)

extern s32 Vec3_Equal(struct Vector3 *a, struct Vector3 *b);
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);

void func_ov002_020effb8(struct Vector3 *out, s32 t, struct Vector3 *a, struct Vector3 *b,
                         struct Vector3 *c, struct Vector3 *d)
{
    s32 s0, s1;
    s32 t4, u, u4;
    s32 t2, t3, u2, u3;
    s32 w0, w1, w2, w3;

    if (Vec3_Equal(a, b) != 0 && Vec3_Equal(b, c) != 0 && Vec3_Equal(c, d) != 0) {
        out->x = a->x;
        out->y = a->y;
        out->z = a->z;
        return;
    }
    if (t > 0x1000)
        t = 0x1000;
    t4 = t << 4;
    t2 = FXMUL(t4, t);
    u = 0x1000 - t;
    t3 = FXMUL(t2, t);
    u4 = u << 4;
    u2 = FXMUL(u4, u);
    u3 = FXMUL(u2, u);
    s0 = (t3 + 1) >> 1;
    w0 = _ZN4cstd4fdivEii(u3, 0x6000);
    w3 = _ZN4cstd4fdivEii(t3, 0x6000);
    s1 = ((t2 + 1) >> 1) - s0;
    w2 = (s1 + ((t4 + 1) >> 1)) + 0x2aab;
    w1 = (s0 - t2) + 0xaaab;

    out->x = TERM(a->x, w0) + TERM(b->x, w1) + TERM(c->x, w2) + TERM(d->x, w3);
    out->y = TERM(a->y, w0) + TERM(b->y, w1) + TERM(c->y, w2) + TERM(d->y, w3);
    out->z = TERM(a->z, w0) + TERM(b->z, w1) + TERM(c->z, w2) + TERM(d->z, w3);
}

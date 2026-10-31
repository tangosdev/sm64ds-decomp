#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007Spring.h"

extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void func_02053320(int s, int *a, int *b, int *out);
extern int DotVec3(const Vector3 *a, const Vector3 *b);

/* func_ov007_020c7560's spring step that holds the link's rest length: adds
 * -r5 times the stretch along the unit from - to direction into each end's
 * force (opposite signs), then damps each end's velocity along that
 * direction by r4. */
void func_ov007_020c7d60(Ov007Link *link, int r5, int r4) {
    Vector3 d;
    s64 sq;
    volatile u16 *ime;
    u16 saved;
    int sqrt_raw, len, dot, s, rest, *p_sqrt;
    Vector3 v1, v2;
    int delta;

    SubVec3((Vector3 *)link->from->target, (Vector3 *)link->to->target, (Vector3 *)&d);

    /* x-first in source; mwccarm emits y-first loads (same as Vec3_DistSq) */
    sq = (s64)(d.x) * (d.x);
    sq += (s64)(d.y) * (d.y);
    sq += (s64)(d.z) * (d.z);

    ime = (volatile u16 *)0x4000208;
    saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = 1;
    *(volatile u64 *)0x40002b8 = (u64)sq << 2;
    *ime;
    *ime = saved;

    *(volatile u16 *)0x4000280 = 2;
    *(volatile u64 *)0x4000290 = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000298 = (u64)sq;

    while (*(volatile u16 *)0x40002b0 & 0x8000) {}

    p_sqrt = (int *)0x40002b4;
    rest = link->length;
    sqrt_raw = *p_sqrt;
    len = (sqrt_raw + 1) / 2;

    delta = len - rest;
    r5 = FX_MUL(delta, r5);
    r5 = -r5;

    while (*(volatile u16 *)0x4000280 & 0x8000) {}

    {
        s64 t = (s64)sqrt_raw * *(volatile s64 *)0x40002a0;
        d.x = (int)((t * d.x + (1LL << 44)) >> 45);
        d.y = (int)((t * d.y + (1LL << 44)) >> 45);
        d.z = (int)((t * d.z + (1LL << 44)) >> 45);
    }

    func_02053320(r5, &d.x, &link->from->force.x, &link->from->force.x);
    func_02053320(-r5, &d.x, &link->to->force.x, &link->to->force.x);

    dot = DotVec3(&d, &link->from->velocity);
    v1.x = (int)(((s64)dot * d.x) >> 12);
    v1.y = (int)(((s64)dot * d.y) >> 12);
    v1.z = (int)(((s64)dot * d.z) >> 12);

    dot = DotVec3(&d, &link->to->velocity);
    v2.x = (int)(((s64)dot * d.x) >> 12);
    v2.y = (int)(((s64)dot * d.y) >> 12);
    v2.z = (int)(((s64)dot * d.z) >> 12);

    s = -r4;
    func_02053320(s, &v1.x, &link->from->force.x, &link->from->force.x);
    func_02053320(s, &v2.x, &link->to->force.x, &link->to->force.x);
}

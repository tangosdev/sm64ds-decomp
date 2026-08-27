typedef long long s64;
typedef unsigned long long u64;
typedef unsigned short u16;

struct Vec3 { int x, y, z; };

extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern void func_02053320(int s, int *a, int *b, int *out);
extern int DotVec3(const struct Vec3 *a, const struct Vec3 *b);

void func_ov007_020c7d60(char **self, int r5, int r4)
{
    struct Vec3 d;
    s64 sq;
    volatile u16 *ime;
    u16 saved;
    int sqrt_raw, len, dot, s, rest, *p_sqrt;
    struct Vec3 v1, v2;
    int delta;

    SubVec3((struct Vec3 *)*(void **)self[0],
            (struct Vec3 *)*(void **)self[1],
            &d);

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
    rest = *(int *)&self[2];
    sqrt_raw = *p_sqrt;
    len = (sqrt_raw + 1) / 2;

    delta = len - rest;
    r5 = (int)(((s64)delta * r5 + 0x800) >> 12);
    r5 = -r5;

    while (*(volatile u16 *)0x4000280 & 0x8000) {}

    {
        s64 t = (s64)sqrt_raw * *(volatile s64 *)0x40002a0;
        d.x = (int)((t * d.x + (1LL << 44)) >> 45);
        d.y = (int)((t * d.y + (1LL << 44)) >> 45);
        d.z = (int)((t * d.z + (1LL << 44)) >> 45);
    }

    func_02053320(r5, (int *)&d, (int *)(self[0] + 0x10), (int *)(self[0] + 0x10));
    func_02053320(-r5, (int *)&d, (int *)(self[1] + 0x10), (int *)(self[1] + 0x10));

    dot = DotVec3(&d, (struct Vec3 *)(self[0] + 4));
    v1.x = (int)(((s64)dot * d.x) >> 12);
    v1.y = (int)(((s64)dot * d.y) >> 12);
    v1.z = (int)(((s64)dot * d.z) >> 12);

    dot = DotVec3(&d, (struct Vec3 *)(self[1] + 4));
    v2.x = (int)(((s64)dot * d.x) >> 12);
    v2.y = (int)(((s64)dot * d.y) >> 12);
    v2.z = (int)(((s64)dot * d.z) >> 12);

    s = -r4;
    func_02053320(s, (int *)&v1, (int *)(self[0] + 0x10), (int *)(self[0] + 0x10));
    func_02053320(s, (int *)&v2, (int *)(self[1] + 0x10), (int *)(self[1] + 0x10));
}

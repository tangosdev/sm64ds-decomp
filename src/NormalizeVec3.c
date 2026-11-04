#include "types.h"
void NormalizeVec3(const Vector3 *v, Vector3 *out) {
    s64 sq;
    volatile u16 *ime;
    u16 saved;
    int len;
    s64 recip;
    s64 scale;
    s64 prod;

    sq = (s64)v->x * v->x;
    sq += (s64)v->y * v->y;
    sq += (s64)v->z * v->z;

    *(volatile u16 *)0x4000280 = 2;
    *(volatile u64 *)0x4000290 = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000298 = sq;

    ime = (volatile u16 *)0x4000208;
    saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = 1;
    *(volatile u64 *)0x40002b8 = (u64)sq << 2;
    *ime;
    *ime = saved;

    while (*(volatile u16 *)0x40002b0 & 0x8000) {}
    len = *(volatile int *)0x40002b4;
    while (*(volatile u16 *)0x4000280 & 0x8000) {}
    recip = *(volatile s64 *)0x40002a0;

    scale = recip * (s64)len;

    prod = scale * (s64)v->x;
    out->x = (int)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)v->y;
    out->y = (int)((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)v->z;
    out->z = (int)((prod + (1LL << 44)) >> 45);
}

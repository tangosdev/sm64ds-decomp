/* Normalises a run of Vector3_16s to unit length using the DS hardware divider
   and square-root units, pipelined: the length of vector n+1 is queued into the
   square-root unit while the reciprocal for vector n is still in flight, so the
   loop body always consumes the results started on the previous pass. That is
   why the first length is set up before the loop and the last one is consumed
   after it.

   0x04000280 DIVCNT, 0x04000290 DIV_NUMER, 0x04000298 DIV_DENOM,
   0x040002a0 DIV_RESULT, 0x040002b0 SQRTCNT, 0x040002b4 SQRT_RESULT,
   0x040002b8 SQRT_PARAM, 0x04000208 IME. Interrupts are masked around each
   square-root start because an interrupt handler that used the unit mid-write
   would corrupt the parameter. */
// @symbol func_ov007_020c368c
#include "common.h"

#pragma opt_propagation off
void func_ov007_020c368c(Vector3_16 *v, int count, int clamp)
{
    int i;
    int len2;
    s64 len2w;
    u64 numer;
    u16 ime_saved;
    Vector3_16 *next;
    int nextLen2;
    s64 nextLen2w;
    int root;
    s64 rootw;
    s64 recip;
    s64 scale;
    s64 prod;
    volatile u16 *ime;

    i = 1;
    len2 = v->x * v->x + v->y * v->y + v->z * v->z;
    len2w = len2;
    *(volatile u16 *)0x4000280 = 2;
    numer = (u64)0x1000000 << 32;
    *(volatile u64 *)0x4000290 = numer;
    *(volatile s64 *)0x4000298 = len2w;
    ime = (volatile u16 *)0x4000208;
    ime_saved = *ime;
    *ime = 0;
    *(volatile u16 *)0x40002b0 = (u16)i;
    *(volatile u64 *)0x40002b8 = (u64)len2w << 2;
    *ime;
    *ime = ime_saved;

    if (count > 1) {
        next = v + 1;
        do {
            nextLen2 = next->x * next->x + next->y * next->y + next->z * next->z;
            nextLen2w = nextLen2;
            while (*(volatile u16 *)0x40002b0 & 0x8000) {}
            root = *(volatile int *)0x40002b4;
            ime_saved = *ime;
            *ime = 0;
            *(volatile u64 *)0x40002b8 = (u64)nextLen2w << 2;
            *ime;
            *ime = ime_saved;
            while (*(volatile u16 *)0x4000280 & 0x8000) {}
            recip = *(volatile s64 *)0x40002a0;
            rootw = root;
            *(volatile u64 *)0x4000290 = numer;
            *(volatile s64 *)0x4000298 = nextLen2w;
            scale = recip * rootw;
            v->x = (s16)((scale * v->x + (1LL << 44)) >> 45);
            v->y = (s16)((scale * v->y + (1LL << 44)) >> 45);
            v->z = (s16)((scale * v->z + (1LL << 44)) >> 45);
            if (clamp != 0) {
                if (v->x > 0xfff) v->x = (s16)0xfff;
                if (v->y > 0xfff) v->y = (s16)0xfff;
                if (v->z > 0xfff) v->z = (s16)0xfff;
            }
            v = next;
            next = next + 1;
            i = i + 1;
        } while (i < count);
    }

    while (*(volatile u16 *)0x40002b0 & 0x8000) {}
    root = *(volatile int *)0x40002b4;
    while (*(volatile u16 *)0x4000280 & 0x8000) {}
    recip = *(volatile s64 *)0x40002a0;
    scale = recip * (s64)root;
    prod = scale * v->x;
    v->x = (s16)((prod + (1LL << 44)) >> 45);
    prod = scale * v->y;
    v->y = (s16)((prod + (1LL << 44)) >> 45);
    prod = scale * v->z;
    v->z = (s16)((prod + (1LL << 44)) >> 45);
    if (clamp == 0)
        return;
    if (v->x > 0xfff) v->x = 0xfff;
    if (v->y > 0xfff) v->y = 0xfff;
    if (v->z > 0xfff) v->z = 0xfff;
}

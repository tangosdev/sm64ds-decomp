//cpp
#include "common.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"


/* Normalises a run of Vector3_16s to unit length using the DS hardware divider
   and square-root units, pipelined: the length of vector n+1 is queued into the
   square-root unit while the reciprocal for vector n is still in flight, so the
   loop body always consumes the results started on the previous pass. That is
   why the first length is set up before the loop and the last one is consumed
   after it. */

void func_ov007_020c368c(Vector3_16 *v, int count, int clamp) {
    int i;
    s64 len2;
    Vector3_16 *next;
    s64 nextLen2;
    int root;
    s64 recip;
    s64 scale;

    len2 = v->x * v->x + v->y * v->y + v->z * v->z;
    CP_SetDiv64_64((u64)0x1000000 << 32, len2);
    CP_SetSqrt64((u64)len2 << 2);

    if (count > 1) {
        i = 1;
        next = v + 1;
        do {
            nextLen2 = next->x * next->x + next->y * next->y + next->z * next->z;
            CP_WaitSqrt();
            root = CP_GetSqrtResultImm32();
            CP_SetSqrtImm64((u64)nextLen2 << 2);
            CP_WaitDiv();
            recip = CP_GetDivResultImm64();
            CP_SetDivImm64_64((u64)0x1000000 << 32, nextLen2);
            scale = recip * root;
            v->x = (s16)((scale * v->x + (1LL << 44)) >> 45);
            v->y = (s16)((scale * v->y + (1LL << 44)) >> 45);
            v->z = (s16)((scale * v->z + (1LL << 44)) >> 45);
            if (clamp != 0) {
                if (v->x > 0xfff)
                    v->x = 0xfff;
                if (v->y > 0xfff)
                    v->y = 0xfff;
                if (v->z > 0xfff)
                    v->z = 0xfff;
            }
            v = next;
            next = next + 1;
            i++;
        } while (i < count);
    }

    CP_WaitSqrt();
    root = CP_GetSqrtResultImm32();
    CP_WaitDiv();
    recip = CP_GetDivResultImm64();
    scale = recip * root;
    v->x = (s16)((scale * v->x + (1LL << 44)) >> 45);
    v->y = (s16)((scale * v->y + (1LL << 44)) >> 45);
    v->z = (s16)((scale * v->z + (1LL << 44)) >> 45);
    if (clamp != 0) {
        if (v->x > 0xfff)
            v->x = 0xfff;
        if (v->y > 0xfff)
            v->y = 0xfff;
        if (v->z > 0xfff)
            v->z = 0xfff;
    }
}


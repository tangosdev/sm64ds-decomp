#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx.h"
#include "Ov007Spring.h"
#include "math/CalcLib.h"

extern void VEC_MultAdd(int s, int *a, int *b, int *out);

/* func_ov007_020c7560's spring step that holds the link's rest length: adds
 * -r5 times the stretch along the unit from - to direction into each end's
 * force (opposite signs), then damps each end's velocity along that
 * direction by r4. */
void func_ov007_020c7d60(Ov007Link *link, int r5, int r4) {
    Vector3 d;
    s64 sq;
    int sqrt_raw, len, dot, s, rest;
    Vector3 v1, v2;
    int delta;

    VEC_Subtract((Vector3 *)link->from->target, (Vector3 *)link->to->target, &d);

    /* x-first in source; mwccarm emits y-first loads (same as Vec3_DistSq) */
    sq = (s64)(d.x) * (d.x);
    sq += (s64)(d.y) * (d.y);
    sq += (s64)(d.z) * (d.z);

    CP_SetSqrt64((u64)sq << 2);
    CP_SetDiv64_64((u64)0x1000000 << 32, (u64)sq);

    CP_WaitSqrt();

    rest = link->length;
    sqrt_raw = CP_GetSqrtResultImm32();
    len = (sqrt_raw + 1) / 2;

    delta = len - rest;
    r5 = FX_MUL(delta, r5);
    r5 = -r5;

    CP_WaitDiv();

    {
        /* The register read, not CP_GetDivResultImm64(): the inline's
           return temporary takes a different register pair. */
        s64 t = (s64)sqrt_raw * *(volatile s64 *)REG_DIV_RESULT_ADDR;
        d.x = (int)((t * d.x + (1LL << 44)) >> 45);
        d.y = (int)((t * d.y + (1LL << 44)) >> 45);
        d.z = (int)((t * d.z + (1LL << 44)) >> 45);
    }

    VEC_MultAdd(r5, &d.x, &link->from->force.x, &link->from->force.x);
    VEC_MultAdd(-r5, &d.x, &link->to->force.x, &link->to->force.x);

    dot = VEC_DotProduct(&d, &link->from->velocity);
    v1.x = (int)(((s64)dot * d.x) >> 12);
    v1.y = (int)(((s64)dot * d.y) >> 12);
    v1.z = (int)(((s64)dot * d.z) >> 12);

    dot = VEC_DotProduct(&d, &link->to->velocity);
    v2.x = (int)(((s64)dot * d.x) >> 12);
    v2.y = (int)(((s64)dot * d.y) >> 12);
    v2.z = (int)(((s64)dot * d.z) >> 12);

    s = -r4;
    VEC_MultAdd(s, &v1.x, &link->from->force.x, &link->from->force.x);
    VEC_MultAdd(s, &v2.x, &link->to->force.x, &link->to->force.x);
}

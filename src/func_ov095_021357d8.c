#include "types.h"
// @symbol func_ov095_021357d8
// @emits SeesawBob_OnGroundPounded
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjSeesaw_c::OnGroundPounded - recovered from vtable slot identity */
#pragma opt_propagation off
extern int Vec3_Dist(void *a, void *b);
extern s16 Vec3_HorzAngle(void *a, void *b);
extern s16 data_02082214[];

void SeesawBob_OnGroundPounded(char *a, char *b)
{
    char *b5c = b + 0x5c;
    int dist = Vec3_Dist(a + 0x5c, b5c);
    s16 angle = Vec3_HorzAngle(a + 0x5c, b5c);
    int n, d, idx, prod, cur, v;
    s16 *p;
    a[0x326] = 1;
    n = func_ov095_0213579c(a, b);
    d = AngleDiff(angle, *(s16 *)(a + 0x8e));
    idx = ((u16)(s16)d >> 4) << 1;

    prod = (int)(((s64)dist * n + 0x800) >> 12);
    idx = idx + 1;

    p = (s16 *)(((s64)(int)(a + 0x8c)));
    cur = *p;
    v = (int)(((s64)prod * data_02082214[idx] + 0x800) >> 12);
    v = (v + ((unsigned)(v >> 11) >> 20)) << 4;
    *p = (s16)(cur + (v >> 16));
    if (*(s16 *)(a + 0x8c) > 0x2000)
        *(s16 *)(a + 0x8c) = 0x2000;
    if (*(s16 *)(a + 0x8c) < -0x2000)
        *(s16 *)(a + 0x8c) = -0x2000;
}

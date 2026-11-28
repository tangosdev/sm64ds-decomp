/* NitroSDK fx_vec.c: fixed-point vector arithmetic. VEC_MultSubtract and
 * VEC_Fx16Distance are dead-stripped in this ROM. mwccarm emits the bodies
 * in reverse source order. */
#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx_vec.h"

/* c = a + b, per axis (NitroSDK fx_vec.c). */

void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *c) {
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    c->z = a->z + b->z;
}

/* c = a - b, per axis (NitroSDK fx_vec.c). */

void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *c) {
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    c->z = a->z - b->z;
}

/* a . b, rounded back to 20.12 (NitroSDK fx_vec.c). */

fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b) {
    long long sum = (long long)a->x * b->x + (long long)a->y * b->y + (long long)a->z * b->z;
    return (fx32)((sum + 0x800) >> 12);
}

/* a x b, each component rounded back to 20.12 (NitroSDK fx_vec.c). */
void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out) {
    long long cx = (long long)a->y * b->z - (long long)a->z * b->y;
    long long cy = (long long)a->z * b->x - (long long)a->x * b->z;
    long long cz = (long long)a->x * b->y - (long long)a->y * b->x;
    out->x = (int)((cx + 0x800) >> 12);
    out->y = (int)((cy + 0x800) >> 12);
    out->z = (int)((cz + 0x800) >> 12);
}

/* |v|: the squared length through the hardware square root, rounded back to
   20.12 (NitroSDK fx_vec.c). */
fx32 VEC_Mag(const VecFx32 *v) {
    s64 sq = (s64)v->x * v->x;
    sq += (s64)v->y * v->y;
    sq += (s64)v->z * v->z;
    sq <<= 2;
    CP_SetSqrt64((u64)sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

/* |v| for a 4.12 vector: the squared length through the hardware square
   root (NitroSDK fx_vec.c). */
fx32 VEC_Fx16Mag(const VecFx16 *v) {
    s64 sq = v->x * v->x;
    sq += v->y * v->y;
    sq += v->z * v->z;
    sq <<= 2;
    CP_SetSqrt64(sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

/* v / |v|: 1 / |v|^2 in the divider while the square-root unit takes |v|,
   then each component times their product (NitroSDK fx_vec.c). */
void VEC_Normalize(const VecFx32 *v, VecFx32 *out) {
    s64 sq;
    int len;
    s64 recip;
    s64 scale;
    s64 prod;

    sq = (s64)v->x * v->x;
    sq += (s64)v->y * v->y;
    sq += (s64)v->z * v->z;

    CP_SetDiv64_64((u64)0x1000000 << 32, sq);
    sq <<= 2;
    CP_SetSqrt64(sq);

    len = CP_GetSqrtResult32();
    recip = CP_GetDivResult64();

    scale = recip * len;

    prod = scale * v->x;
    out->x = ((prod + (1LL << 44)) >> 45);
    prod = scale * v->y;
    out->y = ((prod + (1LL << 44)) >> 45);
    prod = scale * v->z;
    out->z = ((prod + (1LL << 44)) >> 45);
}

/* VEC_Normalize for a 4.12 vector (NitroSDK fx_vec.c). */
void VEC_Fx16Normalize(const VecFx16 *in, VecFx16 *out) {
    u64 sq;
    int sqrtResult;
    s64 divResult;
    s64 scale;
    s64 prod;

    sq = (in->x * in->x);
    sq += (in->y * in->y);
    sq += (in->z * in->z);

    CP_SetDiv64_64((u64)0x1000000 << 32, sq);
    CP_SetSqrt64(sq << 2);

    sqrtResult = CP_GetSqrtResult32();
    divResult = CP_GetDivResult64();

    scale = divResult * (s64)sqrtResult;

    prod = scale * (s64)in->x;
    out->x = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in->y;
    out->y = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in->z;
    out->z = ((prod + (1LL << 44)) >> 45);
}

/* out = s * a + b, per axis. */
void VEC_MultAdd(fx32 s, const VecFx32 *a, const VecFx32 *b, VecFx32 *out) {
    out->x = b->x + (fx32)(((fx64)s * a->x) >> FX32_SHIFT);
    out->y = b->y + (fx32)(((fx64)s * a->y) >> FX32_SHIFT);
    out->z = b->z + (fx32)(((fx64)s * a->z) >> FX32_SHIFT);
}

/* |a - b|: the squared distance through the hardware sqrt unit, rounded
   back to 20.12. */

fx32 VEC_Distance(const VecFx32 *a, const VecFx32 *b) {
    fx32 dx = a->x - b->x;
    long long sq = (long long)dx * dx;
    fx32 dy = a->y - b->y;
    fx32 dz;
    sq += (long long)dy * dy;
    dz = a->z - b->z;
    sq += (long long)dz * dz;
    sq <<= 2;
    CP_SetSqrt64((u64)sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

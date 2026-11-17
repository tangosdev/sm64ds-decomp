#include "types.h"
#include "nitro/fx/fx_mtx33.h"

/* dst = vec * m, the rotation alone (NitroSDK fx_mtx33.c). */
void MTX_MultVec33(const VecFx32 *vec, const MtxFx33 *m, VecFx32 *dst) {
    fx32 x = vec->x;
    fx32 y = vec->y;
    fx32 z = vec->z;

    dst->x =
        (fx32)(((fx64)x * m->m[0][0] + (fx64)y * m->m[1][0] + (fx64)z * m->m[2][0]) >> FX32_SHIFT);
    dst->y =
        (fx32)(((fx64)x * m->m[0][1] + (fx64)y * m->m[1][1] + (fx64)z * m->m[2][1]) >> FX32_SHIFT);
    dst->z =
        (fx32)(((fx64)x * m->m[0][2] + (fx64)y * m->m[1][2] + (fx64)z * m->m[2][2]) >> FX32_SHIFT);
}

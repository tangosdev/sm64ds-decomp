/* a . b, rounded back to 20.12 (NitroSDK fx_vec.c). */
#include "nitro/fx/fx_vec.h"

fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b) {
    long long sum = (long long)a->x * b->x + (long long)a->y * b->y + (long long)a->z * b->z;
    return (fx32)((sum + 0x800) >> 12);
}

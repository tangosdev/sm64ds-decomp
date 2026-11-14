#include "types.h"
#include "nitro/fx/fx_vec.h"

/* a x b, each component rounded back to 20.12 (NitroSDK fx_vec.c). */
void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out) {
    long long cx = (long long)a->y * b->z - (long long)a->z * b->y;
    long long cy = (long long)a->z * b->x - (long long)a->x * b->z;
    long long cz = (long long)a->x * b->y - (long long)a->y * b->x;
    out->x = (int)((cx + 0x800) >> 12);
    out->y = (int)((cy + 0x800) >> 12);
    out->z = (int)((cz + 0x800) >> 12);
}

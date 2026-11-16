/* c = a + b, per axis (NitroSDK fx_vec.c). */
#include "nitro/fx/fx_vec.h"

void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *c) {
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    c->z = a->z + b->z;
}

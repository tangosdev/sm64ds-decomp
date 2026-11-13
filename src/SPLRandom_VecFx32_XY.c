#include "types.h"
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void VEC_Normalize(const struct Vector3 *src, struct Vector3 *dst);

/* A random unit vector in the XY plane: two draws from the SPL LCG, each
 * shifted down 8, for x and y, z = 0, then normalized. The name is pret's
 * SPL's (its spl_random.c, pret-coined); pret's file defines
 * SPLRandom_VecFx32 first, and mwccarm emits in reverse source order. */
void SPLRandom_VecFx32_XY(struct Vector3 *out) {
    s32 s = data_020a4d30;
    s32 x = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    data_020a4d30 = x;
    out->x = x >> 8;
    s = data_020a4d30;
    s32 y = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    data_020a4d30 = y;
    out->y = y >> 8;
    out->z = 0;
    VEC_Normalize(out, out);
}

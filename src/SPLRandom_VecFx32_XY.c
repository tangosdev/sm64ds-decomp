#include "types.h"
#include "nitro/fx/fx_vec.h"
extern u32 gSPLRandomState;

/* A random unit vector in the XY plane: two draws from the SPL LCG, each
 * shifted down 8, for x and y, z = 0, then normalized. The name is pret's
 * SPL's (its spl_random.c, pret-coined); pret's file defines
 * SPLRandom_VecFx32 first, and mwccarm emits in reverse source order. */
void SPLRandom_VecFx32_XY(VecFx32 *out) {
    s32 s = gSPLRandomState;
    s32 x = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    gSPLRandomState = x;
    out->x = x >> 8;
    s = gSPLRandomState;
    s32 y = ((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    gSPLRandomState = y;
    out->y = y >> 8;
    out->z = 0;
    VEC_Normalize(out, out);
}

#ifndef NITRO_FX_FX_MTX33_H
#define NITRO_FX_FX_MTX33_H

/*
 * NitroSDK's 3x3 matrix library (nitro/fx/fx_mtx33.h, libraries/fx/src/
 * fx_mtx33.c), arm9 .text 0x020524f0..0x020527c0, under the SDK's own
 * signatures. Only the routines some caller names are declared so far.
 *
 * These take the SDK's VecFx32 and MtxFx33, not the game's Vector3 and
 * Matrix3x3. In C++, include/math/MtxStack.h includes this header and adds an
 * inline Vector3 / Matrix3x3 overload that forwards here, so no call site
 * casts (issue #5207; fx_vec.h and math/CalcLib.h do the same for VEC_*).
 *
 * Parameters are unnamed on purpose: each named parameter in a declaration
 * costs every C++ file that includes this header a compiler @NNN number.
 */

#include "nitro/fx/fx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* dst = vec * m, the rotation alone; dst may alias vec. */
void MTX_MultVec33(const VecFx32 *, const MtxFx33 *, VecFx32 *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_MTX33_H */

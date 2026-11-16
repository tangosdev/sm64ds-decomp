#ifndef NITRO_FX_FX_MTX43_H
#define NITRO_FX_FX_MTX43_H

/*
 * NitroSDK's 4x3 matrix library (nitro/fx/fx_mtx43.h, libraries/fx/src/
 * fx_mtx43.c), arm9 .text 0x020527c0..0x02052ec8, under the SDK's own
 * signatures. Only the routines some caller names are declared so far; the
 * rest of the library (MTX_Concat43, MTX_Inverse43, ...) is still declared
 * with the game's Matrix4x3 in include/math/MtxStack.h.
 *
 * These take the SDK's VecFx32 and MtxFx43, not the game's Vector3 and
 * Matrix4x3. In C++, include/math/MtxStack.h includes this header and adds an
 * inline Vector3 / Matrix4x3 overload that forwards here, so no call site
 * casts (issue #5207; fx_vec.h and math/CalcLib.h do the same for VEC_*).
 *
 * Parameters are unnamed on purpose: each named parameter in a declaration
 * costs every C++ file that includes this header a compiler @NNN number.
 */

#include "nitro/fx/fx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* dst = vec * m, rotation then translation; dst may alias vec. */
void MTX_MultVec43(const VecFx32 *, const MtxFx43 *, VecFx32 *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_MTX43_H */

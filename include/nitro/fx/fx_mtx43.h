#ifndef NITRO_FX_FX_MTX43_H
#define NITRO_FX_FX_MTX43_H

/*
 * NitroSDK's 4x3 matrix library (nitro/fx/fx_mtx43.h, libraries/fx/src/
 * fx_mtx43.c), arm9 .text 0x020527c0..0x02052ec8, under the SDK's own
 * signatures. Only the routines some caller names are declared so far; the
 * rest of the library (MTX_Concat43, MTX_Identity43_, ...) is still declared
 * with the game's Matrix4x3 in include/math/MtxStack.h.
 *
 * These take the SDK's VecFx32 and MtxFx43, not the game's Vector3 and
 * Matrix4x3. In C++, include/math/MtxStack.h includes this header and adds
 * inline Vector3 / Matrix4x3 overloads that forward here, so no call site
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

/* dst = inverse of src; -1 (dst untouched) when src is singular, else 0.
 * dst may alias src. */
int MTX_Inverse43(const MtxFx43 *, MtxFx43 *); /* s32 in the SDK */

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_MTX43_H */

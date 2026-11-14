#ifndef NITRO_FX_FX_VEC_H
#define NITRO_FX_FX_VEC_H

/*
 * NitroSDK's vector library (nitro/fx/fx_vec.h, libraries/fx/src/fx_vec.c),
 * arm9 .text 0x02053274..0x020538b8, under the SDK's own signatures.
 *
 * These take the SDK's VecFx32 / VecFx16, not the game's Vector3 / Vector3s.
 * The words are the same, but in C++ Vector3 declares a constructor and a
 * destructor, so a Vector3 * does not convert to a VecFx32 *. Callers still
 * reach these through include/math/CalcLib.h's Vector3 spellings; moving a
 * caller onto this header is matching work (issue #5207).
 *
 * Parameters are unnamed on purpose: each named parameter in a declaration
 * costs every C++ file that includes this header a compiler @NNN number.
 */

#include "nitro/fx/fx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ab = a + b and ab = a - b, per axis; ab may alias either operand. */
void VEC_Add(const VecFx32 *, const VecFx32 *, VecFx32 *);
void VEC_Subtract(const VecFx32 *, const VecFx32 *, VecFx32 *);

fx32 VEC_DotProduct(const VecFx32 *, const VecFx32 *);
void VEC_CrossProduct(const VecFx32 *, const VecFx32 *, VecFx32 *);

/* |v|, through the hardware square root. */
fx32 VEC_Mag(const VecFx32 *);
fx32 VEC_Fx16Mag(const VecFx16 *);

/* dst = src / |src|. */
void VEC_Normalize(const VecFx32 *, VecFx32 *);
void VEC_Fx16Normalize(const VecFx16 *, VecFx16 *);

/* dst = a * v1 + v2. */
void VEC_MultAdd(fx32, const VecFx32 *, const VecFx32 *, VecFx32 *);

/* |v1 - v2|. */
fx32 VEC_Distance(const VecFx32 *, const VecFx32 *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_VEC_H */

#ifndef NITRO_GX_G3_UTIL_H
#define NITRO_GX_G3_UTIL_H

#include "types.h"
#include "nitro/fx/fx.h"

/* gx_g3_util.c's matrix builders, under the SDK's names with C linkage
 * (ntrtwl/NitroSDK libraries/gx/src/g3_util.c). Parameters unnamed: a named
 * parameter here renumbers every includer's @NNN.
 *
 * LookAt_ takes the SDK's VecFx32 and MtxFx43. PerspectiveW_'s MtxFx44 is
 * still spelled Matrix4x3, as its callers pass it. The fx32 scalars keep the
 * SDK spelling. Both BOOL flags are int, as the SDK's BOOL is: with a bool
 * PerspectiveW_'s definition does not reproduce the ROM body. */

union Matrix4x3;

#ifdef __cplusplus
extern "C" {
#endif
void G3i_PerspectiveW_(fx32, fx32, fx32, fx32, fx32, fx32, int, union Matrix4x3 *);
void G3i_LookAt_(const VecFx32 *, const VecFx32 *, const VecFx32 *, int, MtxFx43 *);

/* Loads the environment-mapping texture matrix: the SDK's MtxFx44, spelled
 * int * as its definition in src/engine/gx/G3i.cpp and decl_common.h spell
 * it (ntrtwl/NitroSDK nitro/gx/g3_util.h). */
void G3_LoadTexMtxEnv(int *);
#ifdef __cplusplus
}

/* Most of the game's LookAt_ callers hold its Vector3 and Matrix4x3, which
 * carry the same words as VecFx32 and MtxFx43 but are distinct types. This
 * overload takes the game's types and forwards, so the cast is written once
 * here rather than at every call (issue #5207, as math/CalcLib.h does for
 * VEC_* and math/MtxStack.h for MTX_*). No game header is shared by the
 * callers, so it sits here, C++ only.
 *
 * The definition costs every C++ includer one compiler @NNN number per
 * parameter plus three, named or not. */
inline void G3i_LookAt_(const Vector3 *camPos, const Vector3 *camUp, const Vector3 *target,
                        int bPosMode, Matrix4x3 *mtx) {
    G3i_LookAt_((const VecFx32 *)camPos, (const VecFx32 *)camUp, (const VecFx32 *)target, bPosMode,
                (MtxFx43 *)mtx);
}
#endif

#endif

#ifndef NITRO_GX_G3_UTIL_H
#define NITRO_GX_G3_UTIL_H
#ifndef __cplusplus
#define G3_LoadTexMtxEnv _Z16G3_LoadTexMtxEnvPi
#endif

#include "types.h"

/* gx_g3_util.c's matrix builders, under the SDK's names with C linkage
 * (ntrtwl/NitroSDK libraries/gx/src/g3_util.c). Parameters unnamed: a named
 * parameter here renumbers every includer's @NNN.
 *
 * The SDK's VecFx32 and MtxFx43 are this tree's Vector3 and Matrix4x3, and
 * PerspectiveW_'s MtxFx44 is spelled Matrix4x3 as its callers pass it. The
 * fx32 scalars are Fix12i. PerspectiveW_'s BOOL load flag is an int: with a
 * bool the definition does not reproduce the ROM body. LookAt_'s BOOL is a
 * bool, as its callers pass it. */

struct Matrix4x3;

extern "C" void G3i_PerspectiveW_(Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, int, struct Matrix4x3 *);
extern "C" void G3i_LookAt_(const struct Vector3 *, const struct Vector3 *, const struct Vector3 *, bool,
                 struct Matrix4x3 *);

/* Loads the environment-mapping texture matrix: the SDK's MtxFx44, spelled
 * int * as its definition in src/engine/gx/G3i.cpp and decl_common.h spell
 * it (ntrtwl/NitroSDK nitro/gx/g3_util.h). */
void G3_LoadTexMtxEnv(int *);

#endif

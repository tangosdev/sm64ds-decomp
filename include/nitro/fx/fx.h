#ifndef NITRO_FX_FX_H
#define NITRO_FX_FX_H

/*
 * The NitroSDK fixed-point product, under the SDK's own names (nitro/fx/fx.h).
 * fx32 is a 20.12 value in an s32; the product is formed in 64 bits and rounded
 * at bit 11 before the shift back down.
 *
 * Before this header the tree spelled that product out by hand roughly 870 times,
 * plus file-local FMUL / FX_MUL / MULFX / FX_Mul copies.
 *
 * FX_MUL is the SDK's macro form, not its FX_Mul inline, on purpose: a macro is
 * the written-out expression token for token, so a call site cannot change
 * codegen. An inline function also costs every including C++ file a compiler
 * @NNN number, which renames the @NNN objects its __sinit references.
 *
 * Self-contained on purpose: several C sources declare their own Vector3, so
 * pulling in types.h from here would collide with them. The typedefs spell
 * types.h's s32 / s64 directly.
 */

/* include/types.h declares the same four under the same guard (it spells
 * Fix12i as fx32), so a file may include both in either order. */
#ifndef NITRO_FX_SCALAR_TYPES
#define NITRO_FX_SCALAR_TYPES
typedef signed int fx32;
typedef signed long long fx64;
typedef signed long long fx64c;
typedef signed short fx16;
#endif

/* NitroSDK's VecFx32 and VecFx16: plain three-component fixed-point vectors.
 * The game's Vector3 / Vector3s hold the same words but declare a constructor
 * and destructor in C++, which changes how a copy of one is lowered; a POD
 * temporary or view of those words is one of these. */
typedef struct VecFx32 {
    fx32 x, y, z;
} VecFx32;

typedef struct VecFx16 {
    fx16 x, y, z;
} VecFx16;

/* NitroSDK's MtxFx43: a 4x3 fixed-point matrix, row by row. The same 12
 * words as include/private/mtx43.h's Mtx43, a distinct type. */
typedef struct MtxFx43 {
    fx32 m[4][3];
} MtxFx43;

#define FX32_SHIFT 12
#define FX32_ONE   ((fx32)0x00001000L)
#define FX32_CAST(x) ((fx32)(x))

#define FX_MUL(v1, v2) FX32_CAST(((fx64)(v1) * (v2) + 0x800LL) >> FX32_SHIFT)

#ifdef __cplusplus
extern "C" {
#endif

void FX_Init(void); /* fx.c */

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FX_FX_H */

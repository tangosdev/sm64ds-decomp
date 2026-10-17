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

typedef signed int fx32;
typedef signed long long fx64;
typedef signed long long fx64c;

#define FX32_SHIFT 12
#define FX32_CAST(x) ((fx32)(x))

#define FX_MUL(v1, v2) FX32_CAST(((fx64)(v1) * (v2) + 0x800LL) >> FX32_SHIFT)

#endif /* NITRO_FX_FX_H */

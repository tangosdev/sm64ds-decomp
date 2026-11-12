#ifndef NITRO_GX_G2_H
#define NITRO_GX_G2_H

#include "types.h"
#include "nitro/hw/registers.h"

/* gx_g2.c, under the SDK's names with C linkage (ntrtwl/NitroSDK
 * libraries/gx/src/g2.c). Parameters unnamed: a named parameter here
 * renumbers every includer's @NNN.
 *
 * The SDK passes each register block as a u32 address; this tree passes
 * the `volatile unsigned short *` its callers already hold. MtxFx22 is
 * Matrix2x2. G2x_SetBlendBrightnessExt_ is not in this ROM. */

struct Matrix2x2;

#ifdef __cplusplus
extern "C" {
#endif
void G2x_SetBGyAffine_(volatile unsigned short *, struct Matrix2x2 *, int, int, int, int);
void G2x_SetBlendAlpha_(volatile unsigned short *, int, int, int, int);
void G2x_SetBlendBrightness_(volatile unsigned short *, int, int);
void G2x_ChangeBlendBrightness_(volatile unsigned short *, int);
#ifdef __cplusplus
}
#endif

/* The SDK's public wrappers: each passes one engine's register block to the
 * x_ worker above (G2_ for engine A, G2S_ for engine B). The SDK writes them
 * as static inlines; they are macros here because an inline body in a shared
 * header renumbers every includer's @NNN symbols. The expansion is the same
 * call either way. */
#define G2_SetBG2Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_BG2PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2_SetBG3Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_BG3PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2S_SetBG2Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_DB_BG2PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2S_SetBG3Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_DB_BG3PA, (mtx), (centerX), (centerY), (x1), (y1))

#define G2_SetBlendAlpha(plane1, plane2, ev1, ev2)     G2x_SetBlendAlpha_(&REG_BLDCNT, (plane1), (plane2), (ev1), (ev2))
#define G2S_SetBlendAlpha(plane1, plane2, ev1, ev2)     G2x_SetBlendAlpha_(&REG_DB_BLDCNT, (plane1), (plane2), (ev1), (ev2))
#define G2_SetBlendBrightness(plane, brightness)     G2x_SetBlendBrightness_(&REG_BLDCNT, (plane), (brightness))
#define G2S_SetBlendBrightness(plane, brightness)     G2x_SetBlendBrightness_(&REG_DB_BLDCNT, (plane), (brightness))
#define G2_ChangeBlendBrightness(brightness)     G2x_ChangeBlendBrightness_(&REG_BLDCNT, (brightness))
#define G2S_ChangeBlendBrightness(brightness)     G2x_ChangeBlendBrightness_(&REG_DB_BLDCNT, (brightness))

/* Blending off: BLDCNT's effect field back to none. */
#define G2_BlendNone() (REG_BLDCNT = 0)
#define G2S_BlendNone() (REG_DB_BLDCNT = 0)

/* A background's scroll offset: horizontal in BGxOFS bits 0-8, vertical in
   bits 16-24, written as one word. */
#define G2_SetBG0Offset(hOffset, vOffset) (REG_BG0OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG1Offset(hOffset, vOffset) (REG_BG1OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG2Offset(hOffset, vOffset) (REG_BG2OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG3Offset(hOffset, vOffset) (REG_BG3OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG0Offset(hOffset, vOffset) (REG_DB_BG0OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG1Offset(hOffset, vOffset) (REG_DB_BG1OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG2Offset(hOffset, vOffset) (REG_DB_BG2OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG3Offset(hOffset, vOffset) (REG_DB_BG3OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))

#endif

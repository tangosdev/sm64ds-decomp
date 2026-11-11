#ifndef NITRO_GX_G2_H
#define NITRO_GX_G2_H

#include "types.h"

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

#endif

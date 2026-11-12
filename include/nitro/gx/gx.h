#ifndef NITRO_GX_GX_H
#define NITRO_GX_GX_H

/* NitroSDK's display control (gx.h), defined in src/engine/gx/GX_Init.cpp
 * (pret pokediamond arm9/lib/NitroSDK/src/GX.c). The int parameters stand
 * for the SDK's GXDispMode / GXBGMode / GXBG0As enums, and the blank
 * interrupt toggles take and return int for the SDK's BOOL and u32. Parameters stay
 * unnamed: a name in a shared header takes an @NNN in every includer. */

#include "nitro/hw/registers.h"

#ifdef __cplusplus
extern "C" {
#endif

void GX_Init(void);
int GX_HBlankIntr(int);
int GX_VBlankIntr(int);
void GX_DispOff(void);
void GX_DispOn(void);
void GX_SetGraphicsMode(int, int, int);
void GXS_SetGraphicsMode(int);
void GXx_SetMasterBrightness_(volatile unsigned short *, int);

#ifdef __cplusplus
}
#endif

/* The SDK's public wrappers over GXx_SetMasterBrightness_, one per engine.
 * Macros rather than the SDK's static inlines: an inline body in a shared
 * header renumbers every includer's @NNN symbols. */
#define GX_SetMasterBrightness(brightness)     GXx_SetMasterBrightness_(&REG_MASTER_BRIGHT, (brightness))
#define GXS_SetMasterBrightness(brightness)     GXx_SetMasterBrightness_(&REG_DB_MASTER_BRIGHT, (brightness))

#endif

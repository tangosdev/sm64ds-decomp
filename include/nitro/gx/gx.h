#ifndef NITRO_GX_GX_H
#define NITRO_GX_GX_H

/* NitroSDK's display control (gx.h), defined in src/engine/gx/GX_Init.cpp
 * (pret pokediamond arm9/lib/NitroSDK/src/GX.c). The int parameters stand
 * for the SDK's GXDispMode / GXBGMode / GXBG0As enums, and the blank
 * interrupt toggles take and return int for the SDK's BOOL and u32. Parameters stay
 * unnamed: a name in a shared header takes an @NNN in every includer. */

#define GX_LCD_SIZE_X 256
#define GX_LCD_SIZE_Y 192

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
void GXx_SetMasterBrightness_(unsigned short *, int);

#ifdef __cplusplus
}
#endif

#endif

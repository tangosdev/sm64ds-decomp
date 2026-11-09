#ifndef NITRO_GX_GX_H
#define NITRO_GX_GX_H

/* NitroSDK's display control (gx.h), defined in src/engine/gx/GX_Init.cpp
 * (pret pokediamond arm9/lib/NitroSDK/src/GX.c). The int parameters stand
 * for the SDK's GXDispMode / GXBGMode / GXBG0As enums. Parameters stay
 * unnamed: a name in a shared header takes an @NNN in every includer. */

#ifdef __cplusplus
extern "C" {
#endif

void GX_DispOn(void);
void GX_SetGraphicsMode(int, int, int);
void GXS_SetGraphicsMode(int);

#ifdef __cplusplus
}
#endif

#endif
